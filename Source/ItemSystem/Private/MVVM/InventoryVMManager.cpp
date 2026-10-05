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
	if (const TObjectPtr<UVM_ItemBase>* Found = AllVMItems.FindByPredicate([ItemGuid](const UVM_ItemBase* VMItem)
	{
		return VMItem && VMItem->GetGUID() == ItemGuid;
	}))
	{
		return *Found;
	}
	return nullptr;
}

void UInventoryVMManager::CreateVMItem(UItemInstance* ItemInstance)
{
	if (AllVMItems.ContainsByPredicate([ItemInstance](const UVM_ItemBase* VMItem)
	{
		return VMItem && VMItem->GetGUID() == ItemInstance->GetGuid();
	}))
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
			
			AllVMItems.Add(NewVMItem);
			
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

	TObjectPtr<UVM_ItemBase>* VMItem = AllVMItems.FindByPredicate([ItemGuid](const UVM_ItemBase* Item)
	{
		return Item && Item->GetGUID() == ItemGuid;
	});
	
	if (!VMItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveVMItem: No VMItem found with GUID %s."), *ItemGuid.ToString());
		return;
	}
		
	if (OnVMRemoved.IsBound())
	{
		OnVMRemoved.Broadcast(*VMItem);
	}
		
	VMItem->Get()->Deinitialize();
		
	AllVMItems.Remove(*VMItem);
	
}

void UInventoryVMManager::ClearVMData()
{
	if (OnVMDataDeleted.IsBound())
	{
		OnVMDataDeleted.Broadcast();
	}

	for (const auto& Element : AllVMItems)
	{
		if (Element)
		{
			Element->Deinitialize();
		}
	}
		
	AllVMItems.Empty();
		
}

