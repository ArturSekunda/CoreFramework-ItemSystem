// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "MVVM/ViewModels/VM_Extension.h"

#include "Extension/Extension.h"

void UVM_Extension::InitializeVMExtension(UExtension* InExtension)
{
	Extension_Holder = InExtension;
}

void UVM_Extension::Deinitialize()
{
	Extension_Holder = nullptr;
}
