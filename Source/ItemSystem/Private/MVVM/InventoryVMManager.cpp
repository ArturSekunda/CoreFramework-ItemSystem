// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "MVVM/InventoryVMManager.h"

#include "Items/ItemsInstances/ItemInstance.h"
#include "Items/ItemsPrimaryDataAssets/PDA_Item.h"

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
	
	if (ItemInstance->GetItem_PDA())
	{
		UClass* VMClass = ItemInstance->GetItem_PDA()->ItemVMClass.LoadSynchronous();
		
		if (VMClass && VMClass->IsChildOf(UVM_ItemBase::StaticClass()))
		{
			UVM_ItemBase* NewVMItem = NewObject<UVM_ItemBase>(this, VMClass);
			
			NewVMItem->InitializeVMItems(ItemInstance);
			
			AllVMItems.Add(ItemInstance->GetGuid(), NewVMItem);
			
			if (OnVMCreated.IsBound())
			{
				OnVMCreated.Broadcast(NewVMItem);
			}
			
			return;
		}
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateVMItem: ItemInstance %s does not have a valid PDA."), *ItemInstance->GetGuid().ToString());
		return;
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

