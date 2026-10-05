// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Interface/ContainersSerializers.h"
#include "Interface/ExtensionContainer.h"

#include "Items/ItemsData/EditableItemData.h"

#include "UObject/Object.h"
#include "ItemInstance.generated.h"

struct FGameplayTagContainer;
class UItemExtensionContainer;
class UPDA_Upgrade;
class UPDA_Item;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuantityChangedDelegate, int32, NewQuantity, FGuid, ItemGuid);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemExtensionsChangedDelegate);
/**
 * Base class for item instances. You can create your own item instance class by inheriting from this class and adding your own properties and functions.
 */
UCLASS(Blueprintable)
class ITEMSYSTEM_API UItemInstance : public UObject, public IExtensionContainer
{
	GENERATED_BODY()
	
	public: 
	
	virtual void InitializeFromPDA(UPDA_Item* NewPDA, FGuid NewGuid = FGuid::NewGuid());
	
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
	 * Serializer for item extensions. It is used to store all the extensions of the item instance.
	 * Check Interface/ContainersSerializers.h for more info.
	 */
	UPROPERTY(Replicated, VisibleAnywhere,  BlueprintReadOnly, Category = "Item | Extensions")
	FItemExtensionContainerList ExtensionSerializer;
	
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
	 * Create a new Guid for the item instance.
	 * Mainly used while validating the item instance before adding it to the inventory. 
	 * It is used to avoid having two items with the same Guid in the inventory.
	 */
	UFUNCTION(BlueprintCallable, Category = "Item | Guid")
	void CreateNewGuid() { Guid = FGuid::NewGuid(); }
	
	UFUNCTION(BlueprintCallable, Category = "Item | Extensions")
	virtual void PrintDebugData() const;
	
	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Item | SaveData")
	void GetSaveData(FItemInstanceSaveData& OutSaveData) const;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Item | SaveData")
	void LoadFromSaveData(const FItemInstanceSaveData& InSaveData);
	
public: // IExtensionContainer interface

	virtual void PrepareDefaultExtensions_Implementation() override;

	virtual bool AddExtensionInstance_Implementation(UExtension* NewExtension) override;
	
	virtual UExtension* AddNewExtensionByClass_Implementation(TSubclassOf<UExtension> ExtensionClass) override;
	
	virtual UExtension* FindExtension_Implementation(TSubclassOf<UExtension> ExtensionClass) const override;
	
	virtual TArray<UExtension*> GetExtensions_BP_Implementation() const override;
	
	virtual void RemoveAllExtensions_Implementation() override;
	
	virtual bool RemoveExtension_Implementation(UExtension* ExtensionToRemove) override;
	
	
public: // Delegates
	
	UPROPERTY(BlueprintAssignable, Category = "Item | Delegates")
	FOnQuantityChangedDelegate OnQuantityChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Item | Delegates")
	FOnItemExtensionsChangedDelegate OnItemExtensionsChanged;
	
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
	FText GetItemDescription() const;
	
	UFUNCTION(BlueprintPure, Category = "Item | Getter")
	bool GetIsStackable() const;
	
	// Returns all category/type tags for this item (e.g. Item.Category.Weapon). Hierarchical matching via HasTag().
	UFUNCTION(BlueprintPure, Category = "Item | Getter")
	FGameplayTagContainer GetItemCategories() const;
	
	UFUNCTION(BlueprintPure, Category = "Item | Getter")
	void GetItemCategoriesAsFText(TArray<FText>& OutCategoriesAsText) const;
	

};
