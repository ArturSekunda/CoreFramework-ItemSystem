// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#include "Wrappers/ExtensionContainer.h"

#include "Items/ItemsExtensions/ItemExtension.h"
#include "Net/UnrealNetwork.h"

UItemExtensionContainer::UItemExtensionContainer()
{
}

void UItemExtensionContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UItemExtensionContainer, ExtensionList);
}

TArray<UItemExtension*> UItemExtensionContainer::GetExtensions_BP() const
{
	TArray<UItemExtension*> OutExtensions;
	for (const FItemExtensionEntry& Entry : ExtensionList.Entries)
	{
		if (Entry.ExtensionInstance)
		{
			OutExtensions.Add(Entry.ExtensionInstance);
		}
	}
	return OutExtensions;
}

UItemExtension* UItemExtensionContainer::FindExtension(TSubclassOf<UItemExtension> ExtensionClass) const
{
	if (!ExtensionClass)
	{
		return nullptr;
	}

	for (const FItemExtensionEntry& Entry : ExtensionList.Entries)
	{
		if (Entry.ExtensionInstance && Entry.ExtensionInstance->IsA(ExtensionClass))
		{
			return Entry.ExtensionInstance;
		}
	}
	return nullptr;
}

bool UItemExtensionContainer::AddExtensionInstance(UItemExtension* NewExtension)
{
	if (!NewExtension)
	{
		return false;
	}
	
	for (const FItemExtensionEntry& Entry : ExtensionList.Entries)
	{
		if (Entry.ExtensionInstance == NewExtension) return false;
	}
	
	NewExtension->Rename(nullptr, this); 
	NewExtension->SetOwningItem(Cast<UItemInstance>(GetOuter()));
	
	ExtensionList.AddExtension(NewExtension);
	
	NewExtension->InitializeExtension();
	
	if (OnItemExtensionsChanged.IsBound())
	{
		OnItemExtensionsChanged.Broadcast();
	}
	
	return true;
}

UItemExtension* UItemExtensionContainer::AddNewExtensionByClass(TSubclassOf<UItemExtension> ExtensionClass)
{
	if (!ExtensionClass) return nullptr;
	if (FindExtension(ExtensionClass)) return nullptr;
	
	UItemExtension* CreatedExtension = NewObject<UItemExtension>(this, ExtensionClass);
	CreatedExtension->SetOwningItem(Cast<UItemInstance>(GetOuter()));
	ExtensionList.AddExtension(CreatedExtension);
	
	if (OnItemExtensionsChanged.IsBound())
	{
		OnItemExtensionsChanged.Broadcast();
	}

	return CreatedExtension;
}

bool UItemExtensionContainer::RemoveExtension(UItemExtension* ExtensionToRemove)
{
	if (!ExtensionToRemove) return false;
	
	ExtensionList.RemoveExtension(ExtensionToRemove);
	
	if (OnItemExtensionsChanged.IsBound())
	{
		OnItemExtensionsChanged.Broadcast();
	}
	return true;
}

void UItemExtensionContainer::RemoveAllExtensions()
{
	ExtensionList.Entries.Empty();
	ExtensionList.MarkArrayDirty();
	
	if (OnItemExtensionsChanged.IsBound())
	{
		OnItemExtensionsChanged.Broadcast();
	}
}

UInventoryExtensionContainer::UInventoryExtensionContainer()
{
}

void UInventoryExtensionContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryExtensionContainer, ExtensionList);
}

TArray<UInventoryExtension*> UInventoryExtensionContainer::GetExtensions_BP() const
{
	TArray<UInventoryExtension*> OutExtensions;
	for (const FInventoryExtensionEntry& Entry : ExtensionList.Entries)
	{
		if (Entry.ExtensionInstance)
		{
			OutExtensions.Add(Entry.ExtensionInstance);
		}
	}
	return OutExtensions;
}

UInventoryExtension* UInventoryExtensionContainer::FindExtension(TSubclassOf<UInventoryExtension> ExtensionClass) const
{
	if (!ExtensionClass)
	{
		return nullptr;
	}

	for (const FInventoryExtensionEntry& Entry : ExtensionList.Entries)
	{
		if (Entry.ExtensionInstance && Entry.ExtensionInstance->IsA(ExtensionClass))
		{
			return Entry.ExtensionInstance;
		}
	}
	return nullptr;
}

bool UInventoryExtensionContainer::AddExtensionInstance(UInventoryExtension* NewExtension)
{
	if (!NewExtension)
	{
		return false;
	}
	
	for (const FInventoryExtensionEntry& Entry : ExtensionList.Entries)
	{
		if (Entry.ExtensionInstance == NewExtension) return false;
	}
	
	NewExtension->Rename(nullptr, this); 
	
	ExtensionList.AddExtension(NewExtension);
	
	NewExtension->InitializeExtension();
	
	return true;
}

UInventoryExtension* UInventoryExtensionContainer::AddNewExtensionByClass(
	TSubclassOf<UInventoryExtension> ExtensionClass)
{
	if (!ExtensionClass) return nullptr;
	if (FindExtension(ExtensionClass)) return nullptr;
	
	UInventoryExtension* CreatedExtension = NewObject<UInventoryExtension>(this, ExtensionClass);
	ExtensionList.AddExtension(CreatedExtension);

	return CreatedExtension;
}

bool UInventoryExtensionContainer::RemoveExtension(UInventoryExtension* ExtensionToRemove)
{
	if (!ExtensionToRemove) return false;
	
	ExtensionList.RemoveExtension(ExtensionToRemove);
	return true;
}

void UInventoryExtensionContainer::RemoveAllExtensions()
{
	ExtensionList.Entries.Empty();
	ExtensionList.MarkArrayDirty();
}

UEquipmentExtensionContainer::UEquipmentExtensionContainer()
{
}

void UEquipmentExtensionContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEquipmentExtensionContainer, ExtensionList);
}


TArray<UEquipmentExtension*> UEquipmentExtensionContainer::GetExtensions_BP() const
{
	TArray<UEquipmentExtension*> OutExtensions;
	for (const FEquipmentExtensionEntry& Entry : ExtensionList.Entries)
	{
		if (Entry.ExtensionInstance)
		{
			OutExtensions.Add(Entry.ExtensionInstance);
		}
	}
	return OutExtensions;
}

UEquipmentExtension* UEquipmentExtensionContainer::FindExtension(
	TSubclassOf<UEquipmentExtension> ExtensionClass) const
{
	if (!ExtensionClass)
	{
		return nullptr;
	}

	for (const FEquipmentExtensionEntry& Entry : ExtensionList.Entries)
	{
		if (Entry.ExtensionInstance && Entry.ExtensionInstance->IsA(ExtensionClass))
		{
			return Entry.ExtensionInstance;
		}
	}
	return nullptr;
}

bool UEquipmentExtensionContainer::AddExtensionInstance(UEquipmentExtension* NewExtension)
{
	if (!NewExtension)
	{
		return false;
	}

	for (const FEquipmentExtensionEntry& Entry : ExtensionList.Entries)
	{
		if (Entry.ExtensionInstance == NewExtension) return false;
	}

	NewExtension->Rename(nullptr, this);

	ExtensionList.AddExtension(NewExtension);

	NewExtension->InitializeExtension();

	return true;
}

UEquipmentExtension* UEquipmentExtensionContainer::AddNewExtensionByClass(
	TSubclassOf<UEquipmentExtension> ExtensionClass)
{
	if (!ExtensionClass) return nullptr;
	if (FindExtension(ExtensionClass)) return nullptr;

	UEquipmentExtension* CreatedExtension = NewObject<UEquipmentExtension>(this, ExtensionClass);
	ExtensionList.AddExtension(CreatedExtension);

	return CreatedExtension;
}

bool UEquipmentExtensionContainer::RemoveExtension(UEquipmentExtension* ExtensionToRemove)
{
	if (!ExtensionToRemove) return false;

	ExtensionList.RemoveExtension(ExtensionToRemove);
	return true;
	
}

void UEquipmentExtensionContainer::RemoveAllExtensions()
{
	ExtensionList.Entries.Empty();
	ExtensionList.MarkArrayDirty();
}
