// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "VM_ItemExtension.generated.h"

class UItemExtension;
/**
 * 
 */
UCLASS()
class ITEMSYSTEM_API UVM_ItemExtension : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
	protected:
	
	TWeakObjectPtr<UItemExtension> ItemExtension_Holder;
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM")
	virtual void InitializeVMItemExtension(UItemExtension* InItemExtension);
};
