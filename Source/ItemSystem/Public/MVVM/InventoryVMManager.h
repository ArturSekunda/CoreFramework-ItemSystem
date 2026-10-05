// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryVMManager.generated.h"


class UItemInstance;
class UVM_ItemBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVMCreatedDelegate, UVM_ItemBase*, VM_ItemBase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVMRemovedDelegate, UVM_ItemBase*, VM_ItemBase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVMDataDeletedDelegate);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ITEMSYSTEM_API UInventoryVMManager : public UActorComponent
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(VisibleAnywhere, Category = "MVVM")
	TArray<TObjectPtr<UVM_ItemBase>> AllVMItems;
	
	virtual void BeginPlay() override;

public:
	UInventoryVMManager();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(BlueprintAssignable, Category = "MVVM")
	FOnVMCreatedDelegate OnVMCreated;
	
	UPROPERTY(BlueprintAssignable, Category = "MVVM")
	FOnVMRemovedDelegate OnVMRemoved;
	
	UPROPERTY(BlueprintAssignable, Category = "MVVM")
	FOnVMDataDeletedDelegate OnVMDataDeleted;
	
	UFUNCTION(BlueprintCallable, Category = "MVVM")
	UVM_ItemBase* GetVMItemByGuid(FGuid ItemGuid) const;
	
	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void CreateVMItem(UItemInstance* ItemInstance);
	
	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void RemoveVMItem(FGuid ItemGuid);
	
	UFUNCTION(BlueprintCallable, Category = "MVVM")
	void ClearVMData();
	
	
};
