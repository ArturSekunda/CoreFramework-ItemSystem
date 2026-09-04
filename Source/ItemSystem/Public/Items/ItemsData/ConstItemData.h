// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "Items/ItemsActors/ItemActor.h"
#include "ItemTags.h"

#include "ConstItemData.generated.h"

class UItemInstance;

/* 
 * You can add more data for your items here.
 * You can also add more item types in EItemType enum.
 * Remember! This file is responsible for NON-Editable data in runtime. Don't make mess!
 */

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None UMETA(DisplayName = "None"),
	Weapon UMETA(DisplayName = "Weapon"),
	Armor UMETA(DisplayName = "Armor"),
	Consumable UMETA(DisplayName = "Consumable"),
	Upgrade UMETA(DisplayName = "Upgrade"),
	
	MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FConstItemAssetData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AssetData")
	TSoftClassPtr<AItemActor> ItemActorClass = AItemActor::StaticClass();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AssetData")
	TSoftObjectPtr<UTexture2D> ItemTexture = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AssetData")
	TSoftClassPtr<UItemInstance> ItemClass;
	
};

USTRUCT(BlueprintType)
struct FConstItemBaseData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Setup | BaseData")
	FText ItemName = FText::GetEmpty();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Setup | BaseData", meta = (Categories = "Item.Category"))
	FGameplayTagContainer ItemTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Setup | BaseData")
	FText ItemDescription = FText::GetEmpty();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Setup | BaseData")
	bool bCanBeStacked = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Setup | BaseData",meta=(EditCondition="bCanBeStacked"))
	int32 MaxStackCount = 1;
	
	/*
	 * All information about item visuality, actor class, texture, and item instance class. 
	 * Set them in the editor. 
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Setup | BaseData")
	FConstItemAssetData ItemAssetData = FConstItemAssetData();
	
	FConstItemBaseData()
	{
		ItemTags.AddTag(TAG_Item_Category_Base);
	}
	
	
};

USTRUCT(BlueprintType)
struct FConstWeaponData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Setup | BaseData")
	float ConstDamage = 0.0f;
	
	// You can here implement your own weapon data like fire rate, ammo type, etc.
	// But remember that this is only for NON-Editable data in runtime.
	
};

USTRUCT(BlueprintType)
struct FConstArmorData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor Setup | BaseData")
	float ConstDefense = 0.0f;
	
	// You can here implement your own armor data like durability, armor type, etc.
	// But remember that this is only for NON-Editable data in runtime.
	
};

USTRUCT(BlueprintType)
struct FConstConsumableData
{
	GENERATED_BODY()
	
	// You can here implement your own consumable data like healing amount, duration, etc.
	// But remember that this is only for NON-Editable data in runtime.
};

