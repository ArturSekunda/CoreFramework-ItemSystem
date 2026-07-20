// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "MVVM/InventoryVMManager.h"

#include "Items/ItemsInstances/ItemInstance.h"

#include "MVVM/ViewModels/VM_ItemBase.h"


UInventoryVMManager::UInventoryVMManager()
{
	PrimaryComponentTick.bCanEverTick = false;

	
}

void UInventoryVMManager::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void UInventoryVMManager::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

UVM_ItemBase* UInventoryVMManager::GetVMItemByGuid(FGuid ItemGuid) const
{
	if (const TObjectPtr<UVM_ItemBase>* Found = AllVMItems.Find(ItemGuid))
	{
		return *Found;
	}
	return nullptr;
}

void UInventoryVMManager::CreateVMItem(UItemInstance* ItemInstance)
{
	if (AllVMItems.Contains(ItemInstance->GetGuid()))
	{
		// Item already exists
		return;
	}

	// Create new VM item
	UVM_ItemBase* NewVMItem = NewObject<UVM_ItemBase>(this);
	
	NewVMItem->InitializeVMItems(ItemInstance);
	
	AllVMItems.Add(ItemInstance->GetGuid(), NewVMItem);
	
	if (OnVMCreated.IsBound())
	{
		OnVMCreated.Broadcast(NewVMItem);
	}
}

void UInventoryVMManager::RemoveVMItem(FGuid ItemGuid)
{
	if (AllVMItems.Contains(ItemGuid))
	{
		UVM_ItemBase* VMItem = AllVMItems[ItemGuid];
		
		if (OnVMRemoved.IsBound())
		{
			OnVMRemoved.Broadcast(VMItem);
		}
		
		AllVMItems.Remove(ItemGuid);
	}
}

void UInventoryVMManager::ClearVMData()
{
	if (OnVMDataDeleted.IsBound())
	{
		OnVMDataDeleted.Broadcast();
	}
		
	AllVMItems.Empty();
		
}

