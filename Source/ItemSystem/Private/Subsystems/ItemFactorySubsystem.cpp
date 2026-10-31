// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "Subsystems/ItemFactorySubsystem.h"

#include "Engine/AssetManager.h"

#include "Equipment/EquipmentPrimaryDataAssets/PDA_Equipment.h"

#include "Inventory/InventoryPrimaryDataAssets/PDA_Inventory.h"


#include "Items/ItemsInstances/ItemInstance.h"
#include "Items/ItemsActors/ItemActor.h"

#include "Items/ItemsPrimaryDataAssets/PDA_Item.h"


namespace
{
	template<typename T>
	T* LoadPDAFromPath(const FString& Path, const TCHAR* Caller)
	{
		if (Path.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("[ItemFactory | LoadPDAFromPath] %s: PDAPath is empty"), Caller);
			return nullptr;
		}

		T* Result = Cast<T>(FSoftObjectPath(Path).TryLoad());
		if (!Result)
		{
			UE_LOG(LogTemp, Warning, TEXT("[ItemFactory | LoadPDAFromPat] %s: failed to load '%s' as %s"),
				Caller, *Path, *T::StaticClass()->GetName());
		}
		return Result;
	}
}

UItemInstance* UItemFactorySubsystem::CreateItemInstanceInternal(UPDA_Item* ItemPDA, UObject* Outer)
{
	if (!HasAuthority())
	{
		return nullptr;
	}
	
	check(ItemPDA);
	
	TSubclassOf<UItemInstance> ItemClass = ItemPDA->ItemBaseData.ItemAssetData.ItemClass.Get();
	
	if (!ItemClass) // If the class is not loaded yet, load it synchronously + warning.
	{
		ITEM_FACTORY_LOG(
			FString::Printf(
				TEXT("ItemClass in PDA '%s' was not loaded yet. Loaded synchronously."),
				*ItemPDA->GetName()),
				EItemFactoryLog::Error);
		ItemClass = ItemPDA->ItemBaseData.ItemAssetData.ItemClass.LoadSynchronous();
	}
	
	if (!ItemClass)
	{
		ITEM_FACTORY_LOG(
			FString::Printf(
				TEXT("ItemClass in PDA '%s' is null and cannot be instantiated!"),
				*ItemPDA->GetName()),
				EItemFactoryLog::Error);
		return nullptr;
	}
	
	if (ItemClass->HasAnyClassFlags(CLASS_Abstract))
	{
		ITEM_FACTORY_LOG(
			FString::Printf(
				TEXT("ItemClass in PDA '%s' points to an ABSTRACT class (%s) and cannot be instantiated!"), 
			*ItemPDA->GetName(),
			*ItemClass->GetName()),
			EItemFactoryLog::Error);
		return nullptr;
	}

	UItemInstance* Instance = NewObject<UItemInstance>(Outer, ItemClass);
	
	if (Instance)
	{
		Instance->InitializeFromPDA(ItemPDA);
	}

	return Instance;
}

UItemInstance* UItemFactorySubsystem::CreateItemInstance(FPrimaryAssetId AssetId, UObject* Outer)
{
	
	if (!AssetId.IsValid())
	{
		ITEM_FACTORY_LOG(
			FString::Printf(
				TEXT("CreateItemInstance: Invalid AssetId: %s"),
				*AssetId.ToString()),
				EItemFactoryLog::Error);
		return nullptr;
	}

	UAssetManager& AssetManager = UAssetManager::Get();
	
	UObject* AssetObject = AssetManager.GetPrimaryAssetObject(AssetId);
	
	if (!AssetObject)
	{
		ITEM_FACTORY_LOG(
			FString::Printf(
				TEXT("PDA for AssetId '%s' was not loaded yet. Loaded synchronously."),
				*AssetId.ToString()),
				EItemFactoryLog::Error);
	}

	UPDA_Item* ItemPDA = Cast<UPDA_Item>(AssetObject);
	if (!ItemPDA)
	{
		
		ITEM_FACTORY_LOG(
			FString::Printf(
				TEXT("PDA for AssetId '%s' was not loaded yet. Loaded synchronously."), 
				*AssetId.ToString()),
				EItemFactoryLog::Error);
		
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(AssetId);
		ItemPDA = Cast<UPDA_Item>(AssetManager.GetStreamableManager().LoadSynchronous(AssetPath));
	}

	if (!ItemPDA)
	{
		ITEM_FACTORY_LOG(
			FString::Printf(
				TEXT("Failed to load PDA for AssetId: %s"),
				*AssetId.ToString()),
				EItemFactoryLog::Error);
		return nullptr;
	}

	if (UItemInstance* Instance = CreateItemInstanceInternal(ItemPDA, Outer))
	{
		return Instance;
	}
	else
	{
		ITEM_FACTORY_LOG(
			FString::Printf(
				TEXT("Failed to create item instance for PDA: %s"),
				*AssetId.ToString()),
				EItemFactoryLog::Error);
		return nullptr;
	}
}

UItemInstance* UItemFactorySubsystem::CreateItemInstanceFromPDA(TSoftObjectPtr<UPDA_Item> SoftPDA, UObject* Outer)
{
	
	if (SoftPDA.IsNull())
	{
		ITEM_FACTORY_LOG(
			FString::Printf(
				TEXT("CreateItemInstanceFromPDA: SoftPDA is null")),
				EItemFactoryLog::Error);
		return nullptr;
	}
	
	UPDA_Item* ItemPDA = SoftPDA.Get();
	
	if (!ItemPDA)
	{
		ITEM_FACTORY_LOG(
			FString::Printf(
				TEXT("PDA for SoftPDA '%s' was not loaded yet. Loaded synchronously."),
				*SoftPDA.ToString()),
				EItemFactoryLog::Error);
		
		ItemPDA = SoftPDA.LoadSynchronous();
	}
	
	if (!ItemPDA)
	{
		ITEM_FACTORY_LOG(
			FString::Printf(
				TEXT("Failed to load PDA: %s"),
				*SoftPDA.ToString()),
				EItemFactoryLog::Error);
		return nullptr;
	}
	
	if (UItemInstance* Instance = CreateItemInstanceInternal(ItemPDA, Outer))
	{
		return Instance;
	}
	else
	{
		ITEM_FACTORY_LOG(
			FString::Printf(
				TEXT("Failed to create item instance for PDA: %s"),
				*SoftPDA.ToString()),
				EItemFactoryLog::Error);
		return nullptr;
	}
	
}

#if !UE_BUILD_SHIPPING

UItemInstance* UItemFactorySubsystem::CreateItemInstanceFromPDAPathName(FString PDAPath, UObject* Outer)
{
	
	if (PDAPath.IsEmpty())
	{
		ITEM_FACTORY_LOG(
			FString::Printf(
				TEXT("CreateItemInstanceFromPDAPathName: PDAPath is empty")),
				EItemFactoryLog::Error);
		return nullptr;
	}
	
	UPDA_Item* ItemPDA = LoadPDAFromPath<UPDA_Item>(PDAPath, TEXT("CreateItemInstanceFromPDAPathName"));

	if (!ItemPDA)
	{
		ITEM_FACTORY_LOG(
			FString::Printf(
				TEXT("Failed to load PDA: %s"),	
				*PDAPath),
				EItemFactoryLog::Error);
		return nullptr;
	}

	if (UItemInstance* Instance = CreateItemInstanceInternal(ItemPDA, Outer))
	{
		return Instance;
	}
	else
	{
		ITEM_FACTORY_LOG(
			FString::Printf(
				TEXT("Failed to create item instance for PDA: %s"),
				*PDAPath),
				EItemFactoryLog::Error);
		return nullptr;
	}
}

#endif

void UItemFactorySubsystem::SpawnItemActor(TSoftObjectPtr<UPDA_Item> SoftPDA, FVector Location, int32 Quantity)
{
	if (!HasAuthority())
	{
		ITEM_FACTORY_LOG(FString::Printf(
			TEXT("SpawnItemActor: No authority to spawn item actor")),
			EItemFactoryLog::Error);
		return;
	}
	
	if (SoftPDA.IsNull())
	{
		ITEM_FACTORY_LOG(FString::Printf(
			TEXT("SpawnItemActor: SoftPDA is null")), 
			EItemFactoryLog::Error);
		return;
	}

	UPDA_Item* ItemPDA = SoftPDA.Get();
	
	if (!ItemPDA)
	{
		ITEM_FACTORY_LOG(FString::Printf(
			TEXT("SpawnItemActor: PDA for SoftPDA '%s' was not loaded yet. Loaded synchronously."),
			*SoftPDA.ToString()),
			EItemFactoryLog::Error);
		
		ItemPDA = SoftPDA.LoadSynchronous();
	}

	if (!ItemPDA)
	{
		ITEM_FACTORY_LOG(FString::Printf(
			TEXT("Failed to load PDA: %s"),
			*SoftPDA.ToString()),
			EItemFactoryLog::Error);
		return;
	}

	UItemInstance* ItemInstance = CreateItemInstanceInternal(ItemPDA, GetTransientPackage());
	if (!ItemInstance)
	{
		ITEM_FACTORY_LOG(FString::Printf(
			TEXT("Failed to create item instance for PDA: %s"),
			*SoftPDA.ToString()),
			EItemFactoryLog::Error);
		return;
	}
	
	ItemInstance->SetItemQuantity(Quantity);
	
	SpawnItemActorFromInstance(ItemInstance, Location);
}


void UItemFactorySubsystem::SpawnItemActorFromInstance(UItemInstance* ItemInstance, FVector Location)
{
	if (!HasAuthority())
	{
		ITEM_FACTORY_LOG(FString::Printf(
			TEXT("SpawnItemActorFromInstance: No authority to spawn item actor")),
			EItemFactoryLog::Error);
		return;
	}
	
	if (!GetWorld())
	{
		ITEM_FACTORY_LOG(FString::Printf(
			TEXT("SpawnItemActorFromInstance: No world context to spawn item actor")),
			EItemFactoryLog::Error);
		return;
	}
	
	if (!ItemInstance || !ItemInstance->GetItem_PDA())
	{
		ITEM_FACTORY_LOG(FString::Printf(
			TEXT("SpawnItemActorFromInstance: Invalid ItemInstance or its PDA is null")),
			EItemFactoryLog::Error);
		return;
	}
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(Location);
	
	TSubclassOf<AItemActor> ActorClass = ItemInstance->GetItem_PDA()->ItemBaseData.ItemAssetData.ItemActorClass.Get();
	
	if (!ActorClass)
	{
		ITEM_FACTORY_LOG(FString::Printf(
				TEXT("SpawnItemActorFromInstance: ActorClass in PDA '%s' was not loaded yet. Loaded synchronously."),
				*ItemInstance->GetItem_PDA()->GetName()),
				EItemFactoryLog::Warning);
		
		ActorClass = ItemInstance->GetItem_PDA()->ItemBaseData.ItemAssetData.ItemActorClass.LoadSynchronous();
	}
	
	if (!ActorClass)
	{
		ITEM_FACTORY_LOG(FString::Printf(
			TEXT("SpawnItemActorFromInstance: ActorClass in PDA '%s' is null and cannot be spawned!"),
			*ItemInstance->GetItem_PDA()->GetName()),
			EItemFactoryLog::Error);
		return;
	}
	
	AItemActor* ItemActor = GetWorld()->SpawnActorDeferred<AItemActor>(
		ActorClass, 
		SpawnTransform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
	
	if (!ItemActor)
	{
		ITEM_FACTORY_LOG(FString::Printf(
			TEXT("Failed to spawn item actor for PDA: %s"),
			*ItemInstance->GetItem_PDA()->GetName()),
			EItemFactoryLog::Error);
		return;
	}
	
	ItemActor->InitializeItemActor(ItemInstance);
	
	ItemActor->FinishSpawning(SpawnTransform);
}

UPDA_Item* UItemFactorySubsystem::CreatePDAFromString_Item(FString PDAPath)
{
	return LoadPDAFromPath<UPDA_Item>(PDAPath, TEXT("CreatePDAFromString_Item"));
}

UPDA_Inventory* UItemFactorySubsystem::CreatePDAFromString_Inventory(FString PDAPath)
{
	return LoadPDAFromPath<UPDA_Inventory>(PDAPath, TEXT("CreatePDAFromString_Inventory"));
}

UPDA_Equipment* UItemFactorySubsystem::CreatePDAFromString_Equipment(FString PDAPath)
{
	return LoadPDAFromPath<UPDA_Equipment>(PDAPath, TEXT("CreatePDAFromString_Equipment"));
}

void UItemFactorySubsystem::PreloadItem(FPrimaryAssetId AssetId, FStreamableDelegate Delegate)
{
	if (!AssetId.IsValid())
	{
		ITEM_FACTORY_LOG(FString::Printf(
			TEXT("PreloadItem: Invalid AssetId: %s"),
			*AssetId.ToString()),
			EItemFactoryLog::Warning);
		return;
	}
	
	UAssetManager& AssetManager = UAssetManager::Get();
	
	AssetManager.LoadPrimaryAsset(AssetId, {"ItemBundle"}, Delegate);
	
	Delegate.ExecuteIfBound();
	
	
	ITEM_FACTORY_LOG(FString::Printf(
		TEXT("Preloading item with AssetId: %s"),
		*AssetId.ToString()), 
		EItemFactoryLog::Info);
}

void UItemFactorySubsystem::UnloadAllItems()
{
	const UCoreFramework_ItemSystem* Settings = GetDefault<UCoreFramework_ItemSystem>();

	if (!Settings || Settings->DefaultPrimaryAssetIdItemType.IsNone())
	{
		ITEM_FACTORY_LOG(TEXT("UnloadAllItems: Failed to get default AssetType from settings."),
			EItemFactoryLog::Error);
		return;
	}

	const FPrimaryAssetType AssetType = Settings->DefaultPrimaryAssetIdItemType;

	ITEM_FACTORY_LOG(FString::Printf(TEXT("Unloading all items of type: %s"), *AssetType.ToString()),
		EItemFactoryLog::Info);

	const int32 UnloadedCount = UAssetManager::Get().UnloadPrimaryAssetsWithType(AssetType);
	
	ITEM_FACTORY_LOG(FString::Printf(TEXT("Unloaded %d items of type: %s"), UnloadedCount, *AssetType.ToString()),
		EItemFactoryLog::Info);
}

bool UItemFactorySubsystem::HasAuthority() const
{
	const UWorld* World = GetWorld();
	
	if (!World || !World->GetAuthGameMode())
	{
		ITEM_FACTORY_LOG(FString::Printf(
			TEXT("No world or no authority. Server only.")), 
			EItemFactoryLog::Warning);
		return false;
	}
	return true;
}

void UItemFactorySubsystem::ITEM_FACTORY_LOG(const FString& Message, EItemFactoryLog IFL) const
{
	switch (IFL) {
		case EItemFactoryLog::Info:
			UE_LOG(LogTemp, Log, TEXT("[ItemFactory] %s"), *Message);
			break;
		case EItemFactoryLog::Warning:
			UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] %s"), *Message);
			break;
		case EItemFactoryLog::Error:
			UE_LOG(LogTemp, Error, TEXT("[ItemFactory] %s"), *Message);
			break;
	}
}
