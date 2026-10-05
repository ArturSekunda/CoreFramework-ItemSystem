// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "MVVM/ViewModels/VM_ItemBase.h"

#include "MVVM/ViewModels/VM_ItemExtension.h"

#include "GameplayTagContainer.h"

#include "Items/ItemsInstances/ItemInstance.h"
#include "Items/ItemsPrimaryDataAssets/PDA_Item.h"

void UVM_ItemBase::InitializeVMItems(UItemInstance* InItemInstance)
{
	ItemInstance_Holder = InItemInstance;
	if (ItemInstance_Holder.IsValid())
	{
		InItemInstance->OnQuantityChanged.AddDynamic(this, &UVM_ItemBase::SetQuantity);
		InItemInstance->OnItemExtensionsChanged.AddDynamic(this, &UVM_ItemBase::RefreshExtensionViewModels);
		
		SetQuantity(ItemInstance_Holder->GetItemQuantity(), ItemInstance_Holder->GetGuid());
		SetItemCategories();
		SetItemDescription(ItemInstance_Holder->GetItemDescription());
		SetItemName(ItemInstance_Holder->GetItemName());
		SetItemIcon(ItemInstance_Holder->GetItem_PDA() ? ItemInstance_Holder->GetItem_PDA()->ItemBaseData.ItemAssetData.ItemIcon.LoadSynchronous() : nullptr);
		SetMaxStackSize(ItemInstance_Holder->GetMaxStack());
		
		RefreshExtensionViewModels();
	}
}

FGameplayTagContainer UVM_ItemBase::GetItemCategories() const
{
	if (ItemInstance_Holder.IsValid())
	{
		return ItemInstance_Holder->GetItemCategories();
	}
	
	return FGameplayTagContainer();
}

void UVM_ItemBase::SetItemCategories()
{
	if (ItemInstance_Holder.IsValid())
	{
		TArray<FText> CategoriesAsText;
		ItemInstance_Holder->GetItemCategoriesAsFText(CategoriesAsText);
		UE_MVVM_SET_PROPERTY_VALUE(ItemCategoriesAsFText, CategoriesAsText);
	}
	else
	{
		UE_MVVM_SET_PROPERTY_VALUE(ItemCategoriesAsFText, TArray<FText>());
	}
}

FGuid UVM_ItemBase::GetGUID() const
{
	if (ItemInstance_Holder.IsValid())
	{
		return ItemInstance_Holder->GetGuid();
	}
	
	return FGuid();
}

void UVM_ItemBase::Deinitialize()
{
	if (ItemInstance_Holder.IsValid())
	{
		ItemInstance_Holder->OnQuantityChanged.RemoveDynamic(this, &UVM_ItemBase::SetQuantity);
		
		ItemInstance_Holder->OnItemExtensionsChanged.RemoveDynamic(this, &UVM_ItemBase::RefreshExtensionViewModels);
	}
}

void UVM_ItemBase::RefreshExtensionViewModels()
{
	if (!ItemInstance_Holder.IsValid()) return;

	TArray<UVM_ItemExtension*> TempExtensionVMs;
	
	TArray<UExtension*> RawExtensions = ItemInstance_Holder->GetExtensions_BP_Implementation();
		
		for (UExtension* RawExtension : RawExtensions)
		{
			if (!RawExtension)
			{
				continue;
			}

			UVM_ItemExtension* NewExtVM = nullptr;
			
			if (RawExtension->GetVM())
			{
				NewExtVM  = NewObject<UVM_ItemExtension>(this, RawExtension->GetVM());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[VM_ItemBase] RefreshExtensionViewModels: Extension '%s' does not have a valid VM class. Please ensure that the extension has a valid VM class set."), *RawExtension->GetName());
				return;
			}
			
			NewExtVM->InitializeVMExtension(RawExtension);
			
			TempExtensionVMs.Add(NewExtVM);
		}
		
	SetItemExtensions(TempExtensionVMs);
}
