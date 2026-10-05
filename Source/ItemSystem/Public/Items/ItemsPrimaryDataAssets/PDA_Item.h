// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "Items/ItemsData/ConstItemData.h"

#include "Settings/CoreFramework_ItemSystem.h"

#include "PDA_Item.generated.h"

class UVM_ItemBase;
class UItemExtension;
/**
 * Base class for item primary data assets. You can create your own primary data asset class by inheriting from this class and adding your own properties and functions.
 * Remember! This class is responsible for NON-Editable data in runtime. Don't make mess!
 */
UCLASS()
class ITEMSYSTEM_API UPDA_Item : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	public:
	
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Item Data | Default Extensions")
	TArray<TObjectPtr<UItemExtension>> DefaultExtensions;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Data | MVVM")
	TSoftClassPtr<UVM_ItemBase> ItemVMClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FConstItemBaseData ItemBaseData = FConstItemBaseData();
	
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		const UCoreFramework_ItemSystem* Settings = GetDefault<UCoreFramework_ItemSystem>();
		
		if (Settings)
		{
			return FPrimaryAssetId(Settings->DefaultPrimaryAssetIdItemType, GetFName());
		}
		
		return FPrimaryAssetId("Item", GetFName());
	}
	
	
	
	
	
};
