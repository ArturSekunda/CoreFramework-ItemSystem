// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ItemFactorySubsystem.generated.h"

class UPDA_Item;
class UItemInstance;
/**
 * 
 */
UCLASS()
class ITEMSYSTEM_API UItemFactorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	UItemInstance* CreateItemInstanceInternal(UPDA_Item* ItemPDA, UObject* Outer);
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Item | Extensions")
	UItemInstance* CreateItemInstance(FPrimaryAssetId AssetId, UObject* Outer);
	
	UFUNCTION(BlueprintCallable, Category = "Item Factory",meta = (DisplayName = "Create Item Instance (from PDA)"))
	UItemInstance* CreateItemInstanceFromPDA(TSoftObjectPtr<UPDA_Item> SoftPDA, UObject* Outer);
};
