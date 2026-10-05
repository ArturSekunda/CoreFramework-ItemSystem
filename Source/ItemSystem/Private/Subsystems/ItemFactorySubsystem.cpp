// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "Subsystems/ItemFactorySubsystem.h"

#include "Engine/AssetManager.h"

#include "Equipment/EquipmentPrimaryDataAssets/PDA_Equipment.h"

#include "Inventory/InventoryPrimaryDataAssets/PDA_Inventory.h"


#include "Items/ItemsInstances/ItemInstance.h"
#include "Items/ItemsPrimaryDataAssets/PDA_Item.h"

UItemInstance* UItemFactorySubsystem::CreateItemInstanceInternal(UPDA_Item* ItemPDA, UObject* Outer)
{
	if (!GetWorld()->GetAuthGameMode())
	{
		return nullptr;
	}
	
	check(ItemPDA);

	TSubclassOf<UItemInstance> ItemClass = ItemPDA->ItemBaseData.ItemAssetData.ItemClass.LoadSynchronous();
	if (!ItemClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] None ItemClass in PDA '%s'. Please set it in your PDA."),
			*ItemPDA->GetName());
		return nullptr;
	}
	
	if (ItemClass->HasAnyClassFlags(CLASS_Abstract))
	{
		UE_LOG(LogTemp, Error, TEXT("[ItemFactory] ItemClass in PDA '%s' points to an ABSTRACT class (%s) and cannot be instantiated!"), 
			*ItemPDA->GetName(), *ItemClass->GetName());
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
	
	if (!GetWorld()->GetAuthGameMode())
	{
		return nullptr;
	}
	
	if (!AssetId.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] Invalid AssetId: %s"),
			*AssetId.ToString());
		return nullptr;
	}

	UAssetManager& AssetManager = UAssetManager::Get();

	UPDA_Item* ItemPDA = Cast<UPDA_Item>(AssetManager.GetPrimaryAssetObject(AssetId));
	if (!ItemPDA)
	{
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(AssetId);
		ItemPDA = Cast<UPDA_Item>(AssetManager.GetStreamableManager().LoadSynchronous(AssetPath));
	}

	if (!ItemPDA)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] Failed to load PDA for: %s"),
			*AssetId.ToString());
		return nullptr;
	}

	return CreateItemInstanceInternal(ItemPDA, Outer);
}

UItemInstance* UItemFactorySubsystem::CreateItemInstanceFromPDA(TSoftObjectPtr<UPDA_Item> SoftPDA, UObject* Outer)
{
	if (!GetWorld()->GetAuthGameMode())
	{
		return nullptr;
	}
	
	if (SoftPDA.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] CreateItemInstanceFromPDA: SoftPDA is null"));
		return nullptr;
	}

	UPDA_Item* ItemPDA = SoftPDA.Get();
	if (!ItemPDA)
	{
		ItemPDA = Cast<UPDA_Item>(SoftPDA.LoadSynchronous());
	}

	if (!ItemPDA)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] Failed to load PDA: %s"),
			*SoftPDA.ToString());
		return nullptr;
	}

	return CreateItemInstanceInternal(ItemPDA, Outer);
}

UItemInstance* UItemFactorySubsystem::CreateItemInstanceFromPDAPathName(FString PDAPath, UObject* Outer)
{
	
	if (!GetWorld()->GetAuthGameMode())
	{
		return nullptr;
	}
	
	if (PDAPath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] CreateItemInstanceFromPDAPathName: PDAPath is empty"));
		return nullptr;
	}

	FSoftObjectPath SoftPath(PDAPath);
	
	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	
	TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestSyncLoad(SoftPath);

	UPDA_Item* ItemPDA = nullptr;
	if (Handle.IsValid())
	{
		ItemPDA = Cast<UPDA_Item>(Handle->GetLoadedAsset());
	}

	if (!ItemPDA)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] Failed to load PDA from path: %s"),
			*PDAPath);
		return nullptr;
	}

	return CreateItemInstanceInternal(ItemPDA, Outer);
}

void UItemFactorySubsystem::SpawnItemActor(TSoftObjectPtr<UPDA_Item> SoftPDA, FVector Location, int32 Quantity)
{
	
	if (!GetWorld()->GetAuthGameMode())
	{
		return;
	}
	
	if (SoftPDA.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] SpawnItemActor: SoftPDA is null"));
		return;
	}

	UPDA_Item* ItemPDA = SoftPDA.Get();
	if (!ItemPDA)
	{
		ItemPDA = Cast<UPDA_Item>(SoftPDA.LoadSynchronous());
	}

	if (!ItemPDA)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] Failed to load PDA: %s"),
			*SoftPDA.ToString());
		return;
	}

	UItemInstance* ItemInstance = CreateItemInstanceInternal(ItemPDA, GetTransientPackage());
	if (!ItemInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] Failed to create item instance for PDA: %s"),
			*SoftPDA.ToString());
		return;
	}
	
	ItemInstance->SetItemQuantity(Quantity);
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(Location);
	

	AItemActor* ItemActor = GetWorld()->SpawnActorDeferred<AItemActor>(
		ItemInstance->GetItem_PDA()->ItemBaseData.ItemAssetData.ItemActorClass.LoadSynchronous(), 
		SpawnTransform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
	
	if (!ItemActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] Failed to spawn item actor for PDA: %s"),
			*SoftPDA.ToString());
		return;
	}
	
	ItemActor->InitializeItemActor(ItemInstance);
	
	ItemActor->FinishSpawning(SpawnTransform);
}


void UItemFactorySubsystem::SpawnItemActorFromInstance(UItemInstance* ItemInstance, FVector Location, int32 Quantity)
{
	
	if (!GetWorld()->GetAuthGameMode())
	{
		return;
	}
	
	if (!ItemInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] SpawnItemActorFromInstance: ItemInstance is null"));
		return;
	}
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(Location);

	AItemActor* ItemActor = GetWorld()->SpawnActorDeferred<AItemActor>(
		ItemInstance->GetItem_PDA()->ItemBaseData.ItemAssetData.ItemActorClass.LoadSynchronous(), 
		SpawnTransform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
	
	if (!ItemActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] Failed to spawn item actor for PDA: %s"),
			*ItemInstance->GetItem_PDA()->GetName());
		return;
	}
	
	ItemActor->InitializeItemActor(ItemInstance);
	
	ItemActor->FinishSpawning(SpawnTransform);
}

UPDA_Item* UItemFactorySubsystem::CreatePDAFromString_Item(FString PDAPath)
{
	
	if (!GetWorld()->GetAuthGameMode())
	{
		return nullptr;
	}
	
	if (PDAPath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] CreatePDAFromString_Item: PDAPath is empty"));
		return nullptr;
	}

	FSoftObjectPath SoftPath(PDAPath);
	
	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	
	TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestSyncLoad(SoftPath);

	if (Handle.IsValid())
	{
		return Cast<UPDA_Item>(Handle->GetLoadedAsset());
	}
	
	return nullptr;
}

UPDA_Inventory* UItemFactorySubsystem::CreatePDAFromString_Inventory(FString PDAPath)
{
 
	if (!GetWorld()->GetAuthGameMode())
	{
		return nullptr;
	}
	
	if (PDAPath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] CreatePDAFromString_Inventory: PDAPath is empty"));
		return nullptr;
	}

	FSoftObjectPath SoftPath(PDAPath);
	
	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	
	TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestSyncLoad(SoftPath);

	if (Handle.IsValid())
	{
		return Cast<UPDA_Inventory>(Handle->GetLoadedAsset());
	}
	
	return nullptr;
}

UPDA_Equipment* UItemFactorySubsystem::CreatePDAFromString_Equipment(FString PDAPath)
{
	if (!GetWorld()->GetAuthGameMode())
	{
		return nullptr;
	}
	
	if (PDAPath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] CreatePDAFromString_Equipment: PDAPath is empty"));
		return nullptr;
	}

	FSoftObjectPath SoftPath(PDAPath);
	
	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	
	TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestSyncLoad(SoftPath);

	if (Handle.IsValid())
	{
		return Cast<UPDA_Equipment>(Handle->GetLoadedAsset());
	}
	
	return nullptr;
}
