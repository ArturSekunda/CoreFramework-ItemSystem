// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "Items/ItemsInstances/ItemInstance.h"

#include "Items/ItemsPrimaryDataAssets/PDA_Item.h"
#include "Interface/ExtensionContainer.h"

#include "Net/UnrealNetwork.h"

#include "Subsystems/ItemFactorySubsystem.h"

void UItemInstance::InitializeFromPDA(UPDA_Item* NewPDA, FGuid NewGuid)
{
	
	if (!NewPDA)
	{
		return;
	}
	
	Item_PDA = NewPDA;
	
	Guid = NewGuid;
	
	Execute_PrepareDefaultExtensions(this);
	
}

void UItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UItemInstance, EditableBaseData);
	DOREPLIFETIME(UItemInstance, Item_PDA);
	DOREPLIFETIME(UItemInstance, ExtensionSerializer); 
}

UPDA_Item* UItemInstance::GetItem_PDA() const
{
	if (Item_PDA)
	{
		return Item_PDA;
	}
	
	return nullptr;
}

void UItemInstance::PrintDebugData() const
{
	UE_LOG(LogTemp, Warning, TEXT("ItemInstance Debug Data:"));
	UE_LOG(LogTemp, Warning, TEXT("Guid: %s"), *Guid.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Quantity: %d"), EditableBaseData.Quantity);
	UE_LOG(LogTemp, Warning, TEXT("Item_PDA: %s"), Item_PDA ? *Item_PDA->GetName() : TEXT("None"));
	UE_LOG(LogTemp, Warning, TEXT("ExtensionContainer:"));
	UE_LOG(LogTemp, Warning, TEXT("Extensions:"));
	
		for (const FItemExtensionEntry& Entry : ExtensionSerializer.Entries)
		{
			UE_LOG(LogTemp, Warning, TEXT(" - %s"), Entry.ExtensionInstance ?*Entry.ExtensionInstance->GetName() : TEXT("None"));
		}
}

void UItemInstance::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
	Super::PostDuplicate(DuplicateMode);
	
	for (FItemExtensionEntry& Entry : ExtensionSerializer.Entries)
	{
		if (IsValid(Entry.ExtensionInstance))
		{
			Entry.ExtensionInstance->SetOwningItem(this);
		}
	}
}

void UItemInstance::GetSaveData_Implementation(FItemInstanceSaveData& OutSaveData) const
{
	OutSaveData.ItemPDAPath = Item_PDA.GetPathName();
	OutSaveData.Guid = Guid;
	OutSaveData.EditableBaseData = EditableBaseData;

	for (const auto& Element : ExtensionSerializer.Entries)
	{
		// Skip extensions that should not be saved
		if (!Element.ExtensionInstance || !Element.ExtensionInstance->bShouldBeSaved)
		{
			continue;
		}
		
		FSaveExtensionData ExtensionData;
			
		Element.ExtensionInstance->GetSaveData(ExtensionData);
			
		OutSaveData.ExtensionsData.Add(ExtensionData);
		
	}
}

void UItemInstance::LoadFromSaveData_Implementation(const FItemInstanceSaveData& InSaveData)
{
	if (InSaveData.ItemPDAPath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Item PDAPath Empty"));
		return;
	}
	
	if (UItemFactorySubsystem* ItemFactorySubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UItemFactorySubsystem>())
	{
		UPDA_Item* LoadedPDA = ItemFactorySubsystem->CreatePDAFromString_Item(InSaveData.ItemPDAPath);
		
		if (!LoadedPDA)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load PDA from path: %s"), *InSaveData.ItemPDAPath);
			return;
		}
		
		Item_PDA = LoadedPDA;
		
		Guid = InSaveData.Guid;
		
		EditableBaseData = InSaveData.EditableBaseData;
		
		// Add extensions from save data
		for (const FSaveExtensionData& ExtensionData : InSaveData.ExtensionsData)
		{
			if (!ExtensionData.ExtensionClass.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("Invalid Extension Class in Save Data"));
				continue;
			}
			
			UClass* ExtensionClass = ExtensionData.ExtensionClass.TryLoadClass<UExtension>();
			
			if (!ExtensionClass)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to load Extension Class: %s"), *ExtensionData.ExtensionClass.ToString());
				continue;
			}
			
			UExtension* NewExtension = NewObject<UExtension>(this, ExtensionClass);
			
			NewExtension->LoadFromSaveData_Implementation(ExtensionData);
			
			Execute_AddExtensionInstance(this, NewExtension);
		}
		
		// Add default extensions from PDA if they are not already present
		for (UItemExtension* Extension : Item_PDA->DefaultExtensions)
		{
			if (!Extension)
			{
				continue;
			}
			
			// Check if the extension is already present in the ExtensionSerializer
			if (Execute_FindExtension(this, Extension->GetClass()))
			{
				continue;
			}
			
			UItemExtension* CopiedExtension = DuplicateObject<UItemExtension>(Extension, this);
			
			Execute_AddExtensionInstance(this, CopiedExtension);
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemFactorySubsystem not found"));
		return;
	}
}

void UItemInstance::PrepareDefaultExtensions_Implementation()
{
	for (UItemExtension* DefaultExtension : Item_PDA->DefaultExtensions)
	{
		if (DefaultExtension)
		{
			UItemExtension* CopiedExt = DuplicateObject<UItemExtension>(DefaultExtension, this);
			
			if (!CopiedExt)
			{
				UE_LOG(LogTemp, Warning, TEXT("UItemInstance::PrepareDefaultExtensions_Implementation: Failed to duplicate extension %s"), *DefaultExtension->GetName());
				continue;
			}
				
			Execute_AddExtensionInstance(this, CopiedExt);
		}
	}
}

bool UItemInstance::AddExtensionInstance_Implementation(UExtension* NewExtension)
{
	if (!NewExtension)
	{
		return false;
	}
	
	for (const FItemExtensionEntry& Entry : ExtensionSerializer.Entries)
	{
		if (Entry.ExtensionInstance == NewExtension) return false;
	}
	UItemExtension* NewExtensionInstance = Cast<UItemExtension>(NewExtension);
	
	if (!NewExtensionInstance)
	{
		return false;
	}
	
	NewExtensionInstance->Rename(nullptr, this); 
	NewExtensionInstance->SetOwningItem(this);
	NewExtensionInstance->InitializeExtension();
	
	ExtensionSerializer.AddExtension(NewExtensionInstance);
	
	if (OnItemExtensionsChanged.IsBound())
	{
		OnItemExtensionsChanged.Broadcast();
	}
	
	return true;
}

UExtension* UItemInstance::AddNewExtensionByClass_Implementation(TSubclassOf<UExtension> ExtensionClass)
{
	if (!ExtensionClass)
	{
		return nullptr;
	}

	ExtensionSerializer.RemoveInvalidEntries();

	if (UExtension* Existing = Execute_FindExtension(this, ExtensionClass))
	{
		return Existing;
	}

	UItemExtension* CreatedExtension = NewObject<UItemExtension>(this, ExtensionClass);
	
	if (!CreatedExtension)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create new extension: %s"), *ExtensionClass->GetName());
		return nullptr;
	}
	
	CreatedExtension->SetOwningItem(this);
	
	CreatedExtension->InitializeExtension();
	
	ExtensionSerializer.AddExtension(CreatedExtension);

	if (OnItemExtensionsChanged.IsBound())
	{
		OnItemExtensionsChanged.Broadcast();
	}

	return CreatedExtension;
}

UExtension* UItemInstance::FindExtension_Implementation(TSubclassOf<UExtension> ExtensionClass) const
{
	if (!ExtensionClass)
	{
		return nullptr;
	}

	for (const FItemExtensionEntry& Entry : ExtensionSerializer.Entries)
	{
		if (IsValid(Entry.ExtensionInstance) && Entry.ExtensionInstance->IsA(ExtensionClass))
		{
			return Entry.ExtensionInstance;
		}
	}
	return nullptr;
}

TArray<UExtension*> UItemInstance::GetExtensions_BP_Implementation() const
{
	TArray<UExtension*> OutExtensions;
	for (const FItemExtensionEntry& Entry : ExtensionSerializer.Entries)
	{
		if (Entry.ExtensionInstance)
		{
			OutExtensions.Add(Entry.ExtensionInstance);
		}
	}
	
	return OutExtensions;
}

void UItemInstance::RemoveAllExtensions_Implementation()
{
	ExtensionSerializer.Entries.Empty();
	ExtensionSerializer.MarkArrayDirty();
	
	if (OnItemExtensionsChanged.IsBound())
	{
		OnItemExtensionsChanged.Broadcast();
	}
}

bool UItemInstance::RemoveExtension_Implementation(UExtension* ExtensionToRemove)
{
	if (!ExtensionToRemove)
	{
		return false;
	}
	
	ExtensionSerializer.RemoveExtension(ExtensionToRemove);
	
	if (OnItemExtensionsChanged.IsBound())
	{
		OnItemExtensionsChanged.Broadcast();
	}
	return true;
}

void UItemInstance::SetItemQuantity(int32 NewQuantity)
{
	EditableBaseData.Quantity = NewQuantity;
	
	if (OnQuantityChanged.IsBound())
	{
		OnQuantityChanged.Broadcast(NewQuantity);
	}
}

int32 UItemInstance::GetItemQuantity() const
{
	return EditableBaseData.Quantity;
}

int32 UItemInstance::GetMaxStack() const
{
	if (Item_PDA)
	{
		return Item_PDA->ItemBaseData.MaxStackCount;
	}
	
	return -1;
}

FText UItemInstance::GetItemName() const
{
	if (Item_PDA)
	{
		return Item_PDA->ItemBaseData.ItemName;
	}
	
	return FText::Format(NSLOCTEXT("ItemSystem", "ItemInstance_GetItemName_NoPDA", "No PDA for {0}"), FText::FromString(GetName()));
}

bool UItemInstance::GetIsStackable() const
{
	if (Item_PDA)
	{
		return Item_PDA->ItemBaseData.bCanBeStacked;
	}
	
	return false;
}

FGameplayTagContainer UItemInstance::GetItemCategories() const
{
	if (Item_PDA)
	{
		return Item_PDA->ItemBaseData.ItemTags;
	}
	
	return FGameplayTagContainer();
}
