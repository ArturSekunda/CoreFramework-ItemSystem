// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ItemFactorySubsystem.generated.h"

class UPDA_Equipment;
class UPDA_Inventory;
class AItemActor;
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
	
	UFUNCTION(BlueprintCallable, Category = "Item Factory",meta = (DisplayName = "Create Item Instance (from PDA)"))
	UItemInstance* CreateItemInstanceFromPDAPathName(FString PDAPath, UObject* Outer);
	
	UFUNCTION(BlueprintCallable, Category = "Item Factory",meta = (DisplayName = "Spawn Item Actor"))
	void SpawnItemActor(TSoftObjectPtr<UPDA_Item> SoftPDA, FVector Location, UObject* Outer, int32 Quantity);
	
	UFUNCTION(BlueprintCallable, Category = "Item Factory",meta = (DisplayName = "Spawn Item Actor (from Instance)"))
	void SpawnItemActorFromInstance(UItemInstance* ItemInstance, FVector Location, int32 Quantity);
	
	UFUNCTION(BlueprintCallable, Category = "Item Factory",meta = (DisplayName = "Create PDA_Item from FString"))
	UPDA_Item* CreatePDAFromString_Item(FString PDAPath);
	
	UFUNCTION(BlueprintCallable, Category = "Item Factory",meta = (DisplayName = "Create PDA_Inventory from FString"))
	UPDA_Inventory* CreatePDAFromString_Inventory(FString PDAPath);
	
	UFUNCTION(BlueprintCallable, Category = "Item Factory",meta = (DisplayName = "Create PDA_Equipment from FString"))
	UPDA_Equipment* CreatePDAFromString_Equipment(FString PDAPath);
};
