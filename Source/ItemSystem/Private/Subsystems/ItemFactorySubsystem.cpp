// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "Subsystems/ItemFactorySubsystem.h"

#include "Engine/AssetManager.h"


#include "Items/ItemsInstances/ItemInstance.h"
#include "Items/ItemsPrimaryDataAssets/PDA_Item.h"

UItemInstance* UItemFactorySubsystem::CreateItemInstanceInternal(UPDA_Item* ItemPDA, UObject* Outer)
{
	check(ItemPDA);

	TSubclassOf<UItemInstance> ItemClass = ItemPDA->ItemBaseData.ItemAssetData.ItemClass.Get();
	if (!ItemClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemFactory] None ItemClass in PDA '%s', using default UItemInstance class"),
			*ItemPDA->GetName());
		ItemClass = UItemInstance::StaticClass();
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
