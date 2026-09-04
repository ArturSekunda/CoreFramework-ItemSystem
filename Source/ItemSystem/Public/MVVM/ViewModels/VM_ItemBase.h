// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "VM_ItemBase.generated.h"

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
	
	UPROPERTY(FieldNotify, Setter, Getter, BlueprintReadOnly, VisibleAnywhere, Category = "Item MVVM")
	int32 Quantity = 0;
	
	UPROPERTY(FieldNotify, Setter, Getter, BlueprintReadOnly, VisibleAnywhere, Category = "Item MVVM")
	TArray<UVM_ItemExtension*> ItemExtensions;
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM")
	virtual void InitializeVMItems(UItemInstance* InItemInstance);
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM | GUID")
	FGuid GetGUID() const;
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM | Quantity")
	void SetQuantity(const int32 NewQuantity)
	{
		UE_MVVM_SET_PROPERTY_VALUE(Quantity, NewQuantity);
	}
	
	int32 GetQuantity() const { return Quantity; }
	
	virtual void Deinitialize();
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM | Extensions")
	void SetItemExtensions(const TArray<UVM_ItemExtension*>& NewExtensions)
	{
		UE_MVVM_SET_PROPERTY_VALUE(ItemExtensions, NewExtensions);
	}

	TArray<UVM_ItemExtension*> GetItemExtensions() const { return ItemExtensions; }
	
	UFUNCTION()
	void RefreshExtensionViewModels();
	
};
