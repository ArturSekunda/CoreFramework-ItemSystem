// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "VM_ItemBase.generated.h"

struct FGameplayTagContainer;
class UVM_ItemExtension;
struct FEditableItemBaseData;
class UItemInstance;
/**
 * 
 */
UCLASS()
class ITEMSYSTEM_API UVM_ItemBase : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
protected:
	
	TWeakObjectPtr<UItemInstance> ItemInstance_Holder;
	
	UPROPERTY(FieldNotify, Getter, BlueprintReadOnly, VisibleAnywhere, Category = "Item MVVM")
	int32 Quantity = 0;
	
	UPROPERTY(FieldNotify, Getter, BlueprintReadOnly, VisibleAnywhere, Category = "Item MVVM")
	UTexture2D* ItemIcon = nullptr;
	
	UPROPERTY(FieldNotify, Getter, BlueprintReadOnly, VisibleAnywhere, Category = "Item MVVM")
	FText ItemName = FText::GetEmpty();
	
	UPROPERTY(FieldNotify, Getter, BlueprintReadOnly, VisibleAnywhere, Category = "Item MVVM")
	FText ItemDescription = FText::GetEmpty();
	
	UPROPERTY(FieldNotify, Getter, BlueprintReadOnly, VisibleAnywhere, Category = "Item MVVM")
	TArray<FText> ItemCategoriesAsFText;
	
	UPROPERTY(FieldNotify, Getter, BlueprintReadOnly, VisibleAnywhere, Category = "Item MVVM")
	int32 MaxStackSize = 1;
	
	UPROPERTY(FieldNotify, Setter, Getter, BlueprintReadOnly, VisibleAnywhere, Category = "Item MVVM")
	TArray<UVM_ItemExtension*> ItemExtensions;
	
public: // Setters
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM | Quantity")
	void SetQuantity(const int32 NewQuantity, const FGuid ItemGuid)
	{
		UE_MVVM_SET_PROPERTY_VALUE(Quantity, NewQuantity);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM | MaxStackSize")
	void SetMaxStackSize(const int32 NewMaxStackSize)
	{
		UE_MVVM_SET_PROPERTY_VALUE(MaxStackSize, NewMaxStackSize);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM | Extensions")
	void SetItemExtensions(const TArray<UVM_ItemExtension*>& NewExtensions)
	{
		UE_MVVM_SET_PROPERTY_VALUE(ItemExtensions, NewExtensions);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM | Icon")
	void SetItemIcon(UTexture2D* NewIcon)
	{
		UE_MVVM_SET_PROPERTY_VALUE(ItemIcon, NewIcon);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM | ItemName")
	void SetItemName(const FText& NewName)
	{
		UE_MVVM_SET_PROPERTY_VALUE(ItemName, NewName);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM | ItemDescription")
	void SetItemDescription(const FText& NewDescription)
	{
		UE_MVVM_SET_PROPERTY_VALUE(ItemDescription, NewDescription);
	}
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM | Categories")
	void SetItemCategories();
	
public: // Getters
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM | GUID")
	FGuid GetGUID() const;
	
	int32 GetQuantity() const { return Quantity; }
	
	UTexture2D* GetItemIcon() const { return ItemIcon; }
	
	FText GetItemName() const { return ItemName; }
	
	FText GetItemDescription() const { return ItemDescription; }
	
	TArray<FText> GetItemCategoriesAsFText() const { return ItemCategoriesAsFText; }
	
	FGameplayTagContainer GetItemCategories() const;
	
	int32 GetMaxStackSize() const { return MaxStackSize; }
	
	TArray<UVM_ItemExtension*> GetItemExtensions() const { return ItemExtensions; }
	
public: // Other
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM")
	virtual void InitializeVMItems(UItemInstance* InItemInstance);
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM")
	virtual void Deinitialize();
	
	UFUNCTION()
	void RefreshExtensionViewModels();
	
};
