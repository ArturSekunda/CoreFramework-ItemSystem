// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "Inventory/InventoryExtension/InventoryExtension.h"
#include "Inventory/InventoryComponent.h"

void UInventoryExtension::SetOwningInventory(UInventoryComponent* InOwner)
{
	OwningInventory = InOwner;
}

UInventoryComponent* UInventoryExtension::GetOwningInventory() const
{
	if (OwningInventory.IsValid())
	{
		return OwningInventory.Get();
	}
		
	return nullptr;
}
