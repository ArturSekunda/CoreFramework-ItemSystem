// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "Equipment/EquipmentExtension/EquipmentExtension.h"
#include "Equipment/EquipmentComponent.h"

void UEquipmentExtension::SetOwningEquipmentComponent(UEquipmentComponent* InOwner)
{
	OwningEquipmentComponent = InOwner;
}

UEquipmentComponent* UEquipmentExtension::GetOwningEquipmentComponent() const
{
	if (OwningEquipmentComponent.IsValid())
	{
		return OwningEquipmentComponent.Get();
	}
		
	return nullptr;
}