// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditableItemData.generated.h"

/* 
 * There's place for EditableData to make files and code more readable. 
 * You can add more editable data for your items here.
 */


// This's base struct for UItemInstance. Please don't change it.
USTRUCT(BlueprintType)
struct FEditableItemBaseData
{
	GENERATED_BODY()
	
	/*
	 * Use this value if you want to make stackable items. If you don't need it just ignore it.
	 * Remember that MaxStackCount is set in ConstItemData.h and it's a non-editable value in runtime.
	 * Also you can create object with higher number of Quantity by ItemFactorySubsystem, where	you can set Quantity by argument.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Setup | BaseData")
	int32 Quantity = 1;
	
};

USTRUCT(BlueprintType)
struct FEditableWeaponData
{
	GENERATED_BODY()
	
	/*
	 * Set Here your own data to be editable in runtime.
	 * I just leaved it empty because maybe you would like to have clean UWeaponInstance class for some reason. :D
	 */
	
};

USTRUCT(BlueprintType)
struct FEditableArmorData
{
	GENERATED_BODY()
	
	/*
	 * Set Here your own data to be editable in runtime.
	 * I just leaved it empty because maybe you would like to have clean UArmorInstance class for some reason. :D
	 */
};
