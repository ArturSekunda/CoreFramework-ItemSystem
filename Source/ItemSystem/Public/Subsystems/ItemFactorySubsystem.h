// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/StreamableManager.h"

#include "Subsystems/GameInstanceSubsystem.h"
#include "ItemFactorySubsystem.generated.h"

class UPDA_Equipment;
class UPDA_Inventory;
class AItemActor;
class UPDA_Item;
class UItemInstance;

UENUM()
enum EItemFactoryLog
{
	Info,
	Warning,
	Error
};

/**
 * 
 */
UCLASS()
class ITEMSYSTEM_API UItemFactorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	UItemInstance* CreateItemInstanceInternal(UPDA_Item* ItemPDA, UObject* Outer);
	
	// Check if the current context has authority (server-side) to perform item creation and spawning.
	bool HasAuthority() const;

	void ITEM_FACTORY_LOG(const FString& Message, EItemFactoryLog IFL) const;
	
public:
	/**
	 * Creates an item instance from a given PrimaryAssetId. This function will load the associated PDA and create an instance of the item.
	 * @param AssetId - The PrimaryAssetId of the item to create an instance for.
	 * @param Outer - The outer object for the new item instance. If null, the transient package will be used.
	 * @return A pointer to the newly created item instance, or nullptr if the creation failed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Factory", meta = (DisplayName = "Create Item Instance"))
	UItemInstance* CreateItemInstance(FPrimaryAssetId AssetId, UObject* Outer);
	
	/**
	 * Creates an item instance from a given PDA.
	 * @param SoftPDA - The soft object pointer to the PDA.
	 * @param Outer - The outer object for the new item instance. If null, the transient package will be used.
	 * @return A pointer to the newly created item instance, or nullptr if the creation failed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Factory",meta = (DisplayName = "Create Item Instance (from PDA)"))
	UItemInstance* CreateItemInstanceFromPDA(TSoftObjectPtr<UPDA_Item> SoftPDA, UObject* Outer);
	

#if !UE_BUILD_SHIPPING
	
	// Debug only.
	UItemInstance* CreateItemInstanceFromPDAPathName(FString PDAPath, UObject* Outer);
	
#endif
	
	UFUNCTION(BlueprintCallable, Category = "Item Factory",meta = (DisplayName = "Spawn Item Actor"))
	void SpawnItemActor(TSoftObjectPtr<UPDA_Item> SoftPDA, FVector Location, int32 Quantity);
	
	UFUNCTION(BlueprintCallable, Category = "Item Factory",meta = (DisplayName = "Spawn Item Actor (from Instance)"))
	void SpawnItemActorFromInstance(UItemInstance* ItemInstance, FVector Location);
	
	UFUNCTION(BlueprintCallable, Category = "Item Factory",meta = (DisplayName = "Create PDA from String (Item)"))
	UPDA_Item* CreatePDAFromString_Item(FString PDAPath);
	
	UFUNCTION(BlueprintCallable, Category = "Item Factory",meta = (DisplayName = "Create PDA from String (Inventory)"))
	UPDA_Inventory* CreatePDAFromString_Inventory(FString PDAPath);
	
	UFUNCTION(BlueprintCallable, Category = "Item Factory",meta = (DisplayName = "Create PDA from String (Equipment)"))
	UPDA_Equipment* CreatePDAFromString_Equipment(FString PDAPath);
	
	/**
	 * Preload an item by its PrimaryAssetId. This will load the item and its associated assets into memory, allowing for faster access later.
	 * @param AssetId - The PrimaryAssetId of the item to preload.
	 * @param Delegate - Optional delegate to call when the item has finished loading.
	 */
	void PreloadItem(FPrimaryAssetId AssetId, FStreamableDelegate Delegate = FStreamableDelegate());
	
	/**
	 * Unload all items from memory.
	 * This will free up memory by unloading all loaded items and their associated assets.
	 * WARNING: Use this function with caution, as it may cause issues if there are still references to the unloaded items in the game.
	 * (Call this if you want to move player from main gameplay to menu)
	 */
	void UnloadAllItems();
};
