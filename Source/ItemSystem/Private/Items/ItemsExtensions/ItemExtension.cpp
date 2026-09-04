// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "Items/ItemsExtensions/ItemExtension.h"
#include "Items/ItemsInstances/ItemInstance.h"

void UItemExtension::SetOwningItem(UItemInstance* InOwner)
{
	OwningItem = InOwner;
}

UItemInstance* UItemExtension::GetOwningItem() const
{
	if (OwningItem.IsValid())
	{
		return OwningItem.Get();
	}

	return nullptr;
}