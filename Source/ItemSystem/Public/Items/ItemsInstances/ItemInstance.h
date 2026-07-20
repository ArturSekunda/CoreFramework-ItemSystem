// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Items/ItemsData/EditableItemData.h"

#include "UObject/Object.h"
#include "ItemInstance.generated.h"

struct FGameplayTagContainer;
class UItemExtensionContainer;
class UPDA_Upgrade;
class UPDA_Item;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuantityChangedDelegate, int32, NewQuantity);
/**
 * Base class for item instances. You can create your own item instance class by inheriting from this class and adding your own properties and functions.
 */
UCLASS(Abstract, Blueprintable)
class ITEMSYSTEM_API UItemInstance : public UObject
{
	GENERATED_BODY()
	
	public: 
	
	UItemInstance();
	
	virtual void InitializeFromPDA(UPDA_Item* NewPDA, FGuid NewGuid = FGuid::NewGuid());
	virtual void InitializeFromSaveFile(UPDA_Item* NewPDA, FGuid NewGuid, FEditableItemBaseData NewEditableBaseData, UItemExtensionContainer* NewExtensionContainer);
	
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	
	protected:
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Item | UUID")
	FGuid Guid;
	
	/*
	 * Check ItemsData/EditableItemData.h for more info.
	 */
	UPROPERTY(Replicated, SaveGame, EditDefaultsOnly, BlueprintReadOnly, Category = "Item | Properties")
	FEditableItemBaseData EditableBaseData = FEditableItemBaseData();
	
	/*
	 * This's Primary Data Asset of the item instance. 
	 * It contains all the non-editable data of the item. 
	 * You can use it to get all the data you need for your item instance.
	 */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Primary Data Asset")
	TObjectPtr<UPDA_Item> Item_PDA;
	
	/*
	 * Wrapper for extensions. It supports multiplayer with serializer.
	 */
	UPROPERTY(Replicated, VisibleAnywhere,  BlueprintReadOnly, Category = "Item | Extensions")
	TObjectPtr<UItemExtensionContainer> ExtensionContainer;
	
	public:

	/**
	 * Use this function to get Primary Data Asset of the item instance.
	 * @returns Primary Data Asset of the item instance.
	 * @return nullptr if the item instance doesn't have a primary data asset.
	 */
	UFUNCTION(BlueprintPure, Category = "Primary Data Asset")
	UPDA_Item* GetItem_PDA() const;
	
	/**
	 * @return Guid of the item instance. It is unique for each item instance.
	 */
	UFUNCTION(BlueprintCallable, Category = "Item | Properties")
	FGuid GetGuid() const { return Guid; }
	
	/**
	 * It is used to store extensions of the item instance.
	 * @return ExtensionContainer of the item instance.
	 */
	UFUNCTION(BlueprintPure, Category = "Item | Extensions")
	UItemExtensionContainer* GetExtensionContainer() const { return ExtensionContainer; }
	
	UFUNCTION(BlueprintCallable, Category = "Item | Extensions")
	virtual void PrintDebugData() const;
	
	
public: // Delegates
	
	UPROPERTY(BlueprintAssignable, Category = "Item | Delegates")
	FOnQuantityChangedDelegate OnQuantityChanged;
	
public: // Getters & Setters
	
	UFUNCTION(BlueprintCallable, Category = "Item | Setter")
	void SetItemQuantity(int32 NewQuantity);
	
	UFUNCTION(BlueprintPure, Category = "Item | Getter")
	int32 GetItemQuantity() const;

	UFUNCTION(BlueprintPure, Category = "Item | Getter")
	int32 GetMaxStack() const;
	
	UFUNCTION(BlueprintPure, Category = "Item | Getter")
	FText GetItemName() const;
	
	UFUNCTION(BlueprintPure, Category = "Item | Getter")
	bool GetIsStackable() const;
	
	// Returns all category/type tags for this item (e.g. Item.Category.Weapon). Hierarchical matching via HasTag().
	UFUNCTION(BlueprintPure, Category = "Item | Getter")
	FGameplayTagContainer GetItemCategories() const;
	

};
