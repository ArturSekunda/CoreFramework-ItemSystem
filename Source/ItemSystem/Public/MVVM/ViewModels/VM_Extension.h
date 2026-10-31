// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "VM_Extension.generated.h"

class UExtension;
/**
 * 
 */
UCLASS()
class ITEMSYSTEM_API UVM_Extension : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
protected:
	
	TWeakObjectPtr<UExtension> Extension_Holder;
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM")
	virtual void InitializeVMExtension(UExtension* InExtension);
	
	UFUNCTION(BlueprintCallable, Category = "Item MVVM")
	virtual void Deinitialize();
	
	UExtension* GetExtension() const { return Extension_Holder.Get(); }
};
