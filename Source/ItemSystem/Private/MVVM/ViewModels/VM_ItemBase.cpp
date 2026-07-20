// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "MVVM/ViewModels/VM_ItemBase.h"

#include "Extension/PDA_Extension.h"

#include "Items/ItemsExtensions/ItemExtension.h"

#include "MVVM/ViewModels/VM_ItemExtension.h"

#include "Wrappers/ExtensionContainer.h"

void UVM_ItemBase::InitializeVMItems(UItemInstance* InItemInstance)
{
	ItemInstance_Holder = InItemInstance;
	if (ItemInstance_Holder.IsValid())
	{
		InItemInstance->OnQuantityChanged.AddDynamic(this, &UVM_ItemBase::SetQuantity);
		SetQuantity(ItemInstance_Holder->GetItemQuantity());
		
		if (UItemExtensionContainer* Container = ItemInstance_Holder->GetExtensionContainer())
		{
			Container->OnItemExtensionsChanged.AddDynamic(this, &UVM_ItemBase::RefreshExtensionViewModels);
		}
		
		RefreshExtensionViewModels();
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

void UVM_ItemBase::RefreshExtensionViewModels()
{
	if (!ItemInstance_Holder.IsValid()) return;

	TArray<UVM_ItemExtension*> TempExtensionVMs;

	if (UItemExtensionContainer* Container = ItemInstance_Holder->GetExtensionContainer())
	{
		TArray<UItemExtension*> RawExtensions = Container->GetExtensions_BP();
		
		for (UItemExtension* RawExtension : RawExtensions)
		{
			if (!RawExtension) continue;

			UClass* ResolvedVMClass = nullptr;
			
			if (UPDA_Extension* ExtensionPDA = RawExtension->GetExtensionPDA())
			{
				if (!ExtensionPDA->ExtensionVMClass.IsNull())
				{
					ResolvedVMClass = ExtensionPDA->ExtensionVMClass.LoadSynchronous();
				}
			}
			
			if (!ResolvedVMClass)
			{
				ResolvedVMClass = UVM_ItemExtension::StaticClass();
			}
			
			UVM_ItemExtension* NewExtVM = NewObject<UVM_ItemExtension>(this, ResolvedVMClass);
			NewExtVM->InitializeVMItemExtension(RawExtension);
			TempExtensionVMs.Add(NewExtVM);
		}
	}
	
	SetItemExtensions(TempExtensionVMs);
}
