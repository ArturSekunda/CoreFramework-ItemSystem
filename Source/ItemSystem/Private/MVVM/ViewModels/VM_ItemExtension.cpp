// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "MVVM/ViewModels/VM_ItemExtension.h"
#include "Items/ItemsExtensions/ItemExtension.h"


void UVM_ItemExtension::InitializeVMItemExtension(UItemExtension* InItemExtension)
{
	ItemExtension_Holder = InItemExtension;
}
