// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "MVVM/ViewModelManager.h"

#include "Equipment/EquipmentComponent.h"

#include "Inventory/InventoryComponent.h"

#include "Items/ItemsInstances/ItemInstance.h"
#include "Items/ItemsPrimaryDataAssets/PDA_Item.h"


#include "MVVM/ViewModels/VM_EquipmentExtension.h"
#include "MVVM/ViewModels/VM_InventoryExtension.h"
#include "MVVM/ViewModels/VM_ItemBase.h"

void UViewModelManager::HandleNewPawn(APawn* NewPawn)
{
	if (UInventoryComponent* Inv = BoundInventory.Get())
	{
		Inv->OnInventoryLoading.RemoveDynamic(this, &UViewModelManager::Handle_LoadedInventory);
	}
	
	if (UEquipmentComponent* Eq = BoundEquipment.Get())
	{
		Eq->OnEquipmentLoading.RemoveDynamic(this, &UViewModelManager::Handle_LoadedEquipment);
	}
	
	BoundInventory = nullptr;
	BoundEquipment = nullptr;
	VMMOwner = NewPawn;

	if (!NewPawn)
	{
		return;
	}
	
	if (OnAllVMItemDataDeleted.IsBound())
	{
		OnAllVMItemDataDeleted.Broadcast();
	}
	
	if (OnAllVMEquipmentExtensionDataDeleted.IsBound())
	{
		OnAllVMEquipmentExtensionDataDeleted.Broadcast();
	}
	
	if (OnAllVMInventoryExtensionDataDeleted.IsBound())
	{
		OnAllVMInventoryExtensionDataDeleted.Broadcast();
	}
	
	ClearVMItemData();
	
	ClearVMExtensions(EExtensionViewModelType::InventoryExtension);
	
	ClearVMExtensions(EExtensionViewModelType::EquipmentExtension);
	

	if (UInventoryComponent* Inv = NewPawn->FindComponentByClass<UInventoryComponent>())
	{
		Inv->OnInventoryLoading.AddDynamic(this, &UViewModelManager::Handle_LoadedInventory);
		BoundInventory = Inv;
	}
	
	if (UEquipmentComponent* Eq = NewPawn->FindComponentByClass<UEquipmentComponent>())
	{
		Eq->OnEquipmentLoading.AddDynamic(this, &UViewModelManager::Handle_LoadedEquipment);
		BoundEquipment = Eq;
	}
	
}

void UViewModelManager::Handle_LoadedInventory(ESaveLoadResult Result)
{
	if (Result == ESaveLoadResult::Success)
	{
		
		UInventoryComponent* InventoryComponent = BoundInventory.Get();
		
		if (!InventoryComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("[ViewModelManager] Handle_LoadedInventory: No InventoryComponent found on owner pawn."));
			return;
		}
		
		TArray<UItemInstance*> InventoryItems;
				
		InventoryComponent->GetInventoryItems(InventoryItems);
		
		TArray<UExtension*> Extensions = InventoryComponent->Execute_GetExtensions_BP(InventoryComponent);
				
		if (OnAllVMItemDataDeleted.IsBound())
		{
			OnAllVMItemDataDeleted.Broadcast();
		}
		
		if (OnAllVMInventoryExtensionDataDeleted.IsBound())
		{
			OnAllVMInventoryExtensionDataDeleted.Broadcast();
		}
				
		ClearVMItemData();
		
		ClearVMExtensions(EExtensionViewModelType::InventoryExtension);
		
		if (!InventoryItems.IsEmpty())
		{
			for (UItemInstance* ItemInstance : InventoryItems)
			{
				CreateVMItem(ItemInstance);
			}
		}
		
		if (!Extensions.IsEmpty())
		{
			for (UExtension* Extension : Extensions)
			{
				CreateVMExtension(Extension);
			}
		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ViewModelManager] Handle_LoadedInventory: Inventory loading failed with result: %d"), static_cast<uint8>(Result));
	}
}

void UViewModelManager::Handle_LoadedEquipment(ESaveLoadResult Result)
{
	if (Result == ESaveLoadResult::Success)
	{
		
		UEquipmentComponent* EquipmentComponent = BoundEquipment.Get();
		
		if (!EquipmentComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("[ViewModelManager] No EquipmentComponent found on owner pawn."));
			return;
		}
		
		TArray<UExtension*> Extensions = EquipmentComponent->Execute_GetExtensions_BP(EquipmentComponent);
		
		if (OnAllVMEquipmentExtensionDataDeleted.IsBound())
		{
			OnAllVMEquipmentExtensionDataDeleted.Broadcast();
		}
		
		ClearVMExtensions(EExtensionViewModelType::EquipmentExtension);
		
		if (!Extensions.IsEmpty())
		{
			for (UExtension* Extension : Extensions)
			{
				CreateVMExtension(Extension);
			}
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ViewModelManager] Handle_LoadedEquipment: Equipment loading failed with result: %d"), static_cast<uint8>(Result));
	}
}

void UViewModelManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (AController* Controller = Cast<AController>(GetOwner()))
	{
		Controller->GetOnNewPawnNotifier().AddUObject(this, &UViewModelManager::HandleNewPawn);
		
		if (APawn* Existing = Controller->GetPawn())
		{
			HandleNewPawn(Existing);
		}
	}
}

void UViewModelManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
	if (BoundInventory.IsValid())
	{
		if (UInventoryComponent* Inv = BoundInventory.Get())
		{
			Inv->OnInventoryLoading.RemoveDynamic(this, &UViewModelManager::Handle_LoadedInventory);
		}
	}
	
	if (BoundEquipment.IsValid())
	{
		if (UEquipmentComponent* Eq = BoundEquipment.Get())
		{
			Eq->OnEquipmentLoading.RemoveDynamic(this, &UViewModelManager::Handle_LoadedEquipment);
		}
	}
	
	if (AController* Controller = Cast<AController>(GetOwner()))
	{
		Controller->GetOnNewPawnNotifier().RemoveAll(this);
	}
	
	Super::EndPlay(EndPlayReason);
}


UViewModelManager::UViewModelManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UVM_ItemBase* UViewModelManager::GetVMItemByGuid(FGuid ItemGuid) const
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

void UViewModelManager::CreateVMItem(UItemInstance* ItemInstance)
{
	if (!ItemInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ViewModelManager] CreateVMItem: ItemInstance is null."));
		return;
	}
	
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
			
			if (OnVMItemCreated.IsBound())
			{
				OnVMItemCreated.Broadcast(NewVMItem);
			}
			
			return;
		}
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ViewModelManager] CreateVMItem: ItemInstance %s does not have a valid PDA."), *ItemInstance->GetGuid().ToString());
		return;
	}
	
}

void UViewModelManager::RemoveVMItem(FGuid ItemGuid)
{

	const int32 Index = AllVMItems.IndexOfByPredicate([ItemGuid](const UVM_ItemBase* Item)
	{	
	return Item && Item->GetGUID() == ItemGuid;
	});
	
	if (Index == INDEX_NONE) 
	{ 
		UE_LOG(LogTemp, Warning, TEXT("[ViewModelManager] RemoveVMItem: No VMItem found with GUID %s."), *ItemGuid.ToString());
		return; 
	}
	
	UVM_ItemBase* VM = AllVMItems[Index];
	
	if (OnVMItemRemoved.IsBound())
	{
		OnVMItemRemoved.Broadcast(VM);
	}
	
	VM->Deinitialize();
	
	AllVMItems.RemoveAt(Index);
	
}

void UViewModelManager::ClearVMItemData()
{
	if (OnAllVMItemDataDeleted.IsBound())
	{
		OnAllVMItemDataDeleted.Broadcast();
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

void UViewModelManager::CreateVMExtension(UExtension* NewExtension)
{
	if (!NewExtension) 
	{ 
		UE_LOG(LogTemp, Warning, TEXT("[ViewModelManager] CreateVMExtension: NewExtension is null."));
		return; 
	}

	if (AllVMInventoryExtensions.ContainsByPredicate([NewExtension](const UVM_InventoryExtension* VM)
	{
		return VM && VM->GetExtension() == NewExtension;
	}))
	{
		// Inventory extension already exists
		return;
	}

	if (AllVMEquipmentExtensions.ContainsByPredicate([NewExtension](const UVM_EquipmentExtension* VM)
	{
		return VM && VM->GetExtension() == NewExtension;
	}))
	{
		// Equipment extension already exists
		return;
	}

	if (NewExtension->IsA(UInventoryExtension::StaticClass()))
	{
		UVM_InventoryExtension* NewVM = NewObject<UVM_InventoryExtension>(this, NewExtension->GetVM());
		
		NewVM->InitializeVMExtension(NewExtension);
		
		AllVMInventoryExtensions.Add(NewVM);
		
		OnVMInventoryExtensionCreated.Broadcast(NewVM);
		
		return;
	}
	else if (NewExtension->IsA(UEquipmentExtension::StaticClass()))
	{
		UVM_EquipmentExtension* NewVM = NewObject<UVM_EquipmentExtension>(this, NewExtension->GetVM());
		
		NewVM->InitializeVMExtension(NewExtension);
		
		AllVMEquipmentExtensions.Add(NewVM);
		
		OnVMEquipmentExtensionCreated.Broadcast(NewVM);
		
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ViewModelManager] CreateVMExtension: Extension %s is neither Inventory nor Equipment."), *GetNameSafe(NewExtension));
	}
}

void UViewModelManager::RemoveVMExtension(UExtension* ExtensionToRemove)
{
	if (!ExtensionToRemove) 
	{ 
		UE_LOG(LogTemp, Warning, TEXT("[ViewModelManager] RemoveVMExtension: ExtensionToRemove is null."));
		return; 
	}

	int32 Index = AllVMInventoryExtensions.IndexOfByPredicate([ExtensionToRemove](const UVM_InventoryExtension* VM)
	{
		return VM && VM->GetExtension() == ExtensionToRemove;
	});

	if (Index != INDEX_NONE)
	{
		UVM_InventoryExtension* VM = AllVMInventoryExtensions[Index];
		
		AllVMInventoryExtensions.RemoveAt(Index);
		
		if (OnVMInventoryExtensionRemoved.IsBound())
		{
			OnVMInventoryExtensionRemoved.Broadcast(VM);
		}
		
		VM->Deinitialize();
		
		return;
	}

	Index = AllVMEquipmentExtensions.IndexOfByPredicate([ExtensionToRemove](const UVM_EquipmentExtension* VM)
	{
		return VM && VM->GetExtension() == ExtensionToRemove;
	});

	if (Index != INDEX_NONE)
	{
		UVM_EquipmentExtension* VM = AllVMEquipmentExtensions[Index];
		
		AllVMEquipmentExtensions.RemoveAt(Index);
		
		if (OnVMEquipmentExtensionRemoved.IsBound())
		{
			OnVMEquipmentExtensionRemoved.Broadcast(VM);
		}
		
		VM->Deinitialize();
		
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[ViewModelManager] RemoveVMExtension: no VM for extension %s"), *GetNameSafe(ExtensionToRemove));
}

void UViewModelManager::ClearVMExtensions(EExtensionViewModelType ExtensionType)
{
	switch (ExtensionType)
	{
	case EExtensionViewModelType::InventoryExtension:
		{
			if (OnAllVMInventoryExtensionDataDeleted.IsBound())
			{
				OnAllVMInventoryExtensionDataDeleted.Broadcast();
			}
			
			for (UVM_InventoryExtension* VM : AllVMInventoryExtensions)
			{
				if (VM)
				{
					VM->Deinitialize();
				}
			}
			
			AllVMInventoryExtensions.Empty();
			
			break;
		}
	case EExtensionViewModelType::EquipmentExtension:
		{
			if (OnAllVMEquipmentExtensionDataDeleted.IsBound())
			{
				OnAllVMEquipmentExtensionDataDeleted.Broadcast();
			}
			
			for (UVM_EquipmentExtension* VM : AllVMEquipmentExtensions)
			{
				if (VM)
				{
					VM->Deinitialize();
				}
			}
			
			AllVMEquipmentExtensions.Empty();
			
			break;
		}
		default:
		{
			UE_LOG(LogTemp, Warning, TEXT("[ViewModelManager] ClearVMExtensions: Unknown ExtensionType %d"), static_cast<int32>(ExtensionType));
			break;
		}
	}
}

