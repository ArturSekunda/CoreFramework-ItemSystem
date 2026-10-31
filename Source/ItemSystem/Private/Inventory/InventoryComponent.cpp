// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "Inventory/InventoryComponent.h"

#include "Interface/ExtensionContainer.h"

#include "Inventory/InventoryExtension/InventoryExtension.h"

#include "Inventory/InventoryPrimaryDataAssets/PDA_Inventory.h"

#include "Items/ItemsInstances/ItemInstance.h"
#include "Items/ItemsPrimaryDataAssets/PDA_Item.h"

#include "Net/UnrealNetwork.h"

#include "Subsystems/ItemFactorySubsystem.h"


UInventoryComponent::UInventoryComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;
	
	bReplicateUsingRegisteredSubObjectList = true;
	SetIsReplicatedByDefault(true);
	
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Prepare default extensions for the Inventory Component.
	// USE: PrepareDefaultExtensions() check "Interface/ExtensionContainer.h" for more information
	// This is called only on the server. You should use this in here when there's no loading from save data.
	// (New gameplay feature)
	/*
	* 
	*if (GetOwnerRole() == ROLE_Authority && bPrepareDefaultExtensionsOnBeginPlay (or something like that))
	*{
	*	Execute_PrepareDefaultExtensions();
	*}
	*/
	
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UInventoryComponent, InventoryContainer);
	DOREPLIFETIME(UInventoryComponent, InventoryExtensionSerializer); 
	
}

void UInventoryComponent::GetInventoryItems(TArray<UItemInstance*>& OutItems)
{
	
	OutItems.Empty();
	
	for (const FInventoryEntry& Entry : InventoryContainer.Entries)
	{
		if (Entry.ItemInstance)
		{
			OutItems.Add(Entry.ItemInstance);
		}
	}
}

void UInventoryComponent::ReplicateItemAndExtensions(UItemInstance* InItemInstance)
{
	AddReplicatedSubObject(InItemInstance);
	
	if (!InItemInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::ReplicateItemAndExtensions: InItemInstance is null"));
		return;
	}
	
	for (UExtension* Ext : InItemInstance->Execute_GetExtensions_BP(InItemInstance))
	{
		if (Ext)
		{
			AddReplicatedSubObject(Ext);
		}
	}
	
}

void UInventoryComponent::RemoveReplicatedItemAndExtensions(UItemInstance* InItemInstance)
{
	RemoveReplicatedSubObject(InItemInstance);
	
	if (!InItemInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::RemoveReplicatedItemAndExtensions: InItemInstance is null"));
		return;
	}
	
	for (UExtension* Ext : InItemInstance->Execute_GetExtensions_BP(InItemInstance))
	{
		if (Ext)
		{
			RemoveReplicatedSubObject(Ext);
		}
	}
}

void UInventoryComponent::PrepareDefaultExtensions_Implementation()
{
	for (TObjectPtr<UInventoryExtension> DefaultExtension : Inventory_PDA->DefaultExtensions)
	{
		if (DefaultExtension)
		{
			UInventoryExtension* CopiedExt = DuplicateObject<UInventoryExtension>(DefaultExtension, this);
			
			if (!CopiedExt)
			{
				UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::PrepareDefaultExtensions_Implementation: Failed to duplicate extension %s"), *DefaultExtension->GetName());
				continue;
			}
			
			Execute_AddExtensionInstance(this, CopiedExt);
		}
	}
}

bool UInventoryComponent::AddExtensionInstance_Implementation(UExtension* NewExtension)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddExtensionInstance_Implementation: Only the server can add extensions!"));
		return false;
	}
	
	if (!NewExtension)
	{
		return false;
	}
	
	for (const FInventoryExtensionEntry& Entry : InventoryExtensionSerializer.Entries)
	{
		if (Entry.ExtensionInstance == NewExtension) return false;
	}
	
	UInventoryExtension* InventoryExt = Cast<UInventoryExtension>(NewExtension);
	
	if (!InventoryExt) 
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddExtensionInstance_Implementation: NewExtension is not a UInventoryExtension!"));
		return false;
	}
	
	NewExtension->Rename(nullptr, this); 
	
	InventoryExt->SetOwningInventory(this);
	
	NewExtension->InitializeExtension();
	
	AddReplicatedSubObject(InventoryExt);
	
	InventoryExtensionSerializer.AddExtension(InventoryExt);
	
	
	return true;
}

UExtension* UInventoryComponent::AddNewExtensionByClass_Implementation(TSubclassOf<UExtension> ExtensionClass)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddNewExtensionByClass_Implementation: Only the server can add extensions!"));
		return nullptr;
	}
	
	if (!ExtensionClass)
	{
		return nullptr;
	}
	

	InventoryExtensionSerializer.RemoveInvalidEntries();

	if (UExtension* Existing = Execute_FindExtension(this, ExtensionClass))
	{
		return Existing;
	}

	UInventoryExtension* CreatedExtension = NewObject<UInventoryExtension>(this, ExtensionClass);
	
	if (!CreatedExtension)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddNewExtensionByClass_Implementation: Failed to create extension of class %s"), *ExtensionClass->GetName());
		return nullptr;
	}
	
	CreatedExtension->Rename(nullptr, this);
	
	CreatedExtension->SetOwningInventory(this);
	
	CreatedExtension->InitializeExtension();
	
	AddReplicatedSubObject(CreatedExtension);
	
	InventoryExtensionSerializer.AddExtension(CreatedExtension);

	return CreatedExtension;
}

UExtension* UInventoryComponent::FindExtension_Implementation(TSubclassOf<UExtension> ExtensionClass) const
{
	
	if (!ExtensionClass)
	{
		return nullptr;
	}

	for (const FInventoryExtensionEntry& Entry : InventoryExtensionSerializer.Entries)
	{
		if (IsValid(Entry.ExtensionInstance) && Entry.ExtensionInstance->IsA(ExtensionClass))
		{
			return Entry.ExtensionInstance;
		}
	}
	return nullptr;
}

TArray<UExtension*> UInventoryComponent::GetExtensions_BP_Implementation() const
{
	
	TArray<UExtension*> OutExtensions;
	for (const FInventoryExtensionEntry& Entry : InventoryExtensionSerializer.Entries)
	{
		if (Entry.ExtensionInstance)
		{
			OutExtensions.Add(Entry.ExtensionInstance);
		}
	}
	return OutExtensions;
}


void UInventoryComponent::RemoveAllExtensions_Implementation()
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::RemoveAllExtensions_Implementation: Only the server can remove extensions!"));
		return;
	}
	
	for (const FInventoryExtensionEntry& Entry : InventoryExtensionSerializer.Entries)
	{
		if (UInventoryExtension* ExtInstance = Entry.ExtensionInstance)
		{
			RemoveReplicatedSubObject(ExtInstance);
		}
	}
	
	InventoryExtensionSerializer.Entries.Empty();
	InventoryExtensionSerializer.MarkArrayDirty();
}

bool UInventoryComponent::RemoveExtension_Implementation(UExtension* ExtensionToRemove)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::RemoveExtension_Implementation: Only the server can remove extensions!"));
		return false;
	}
	
	if (!ExtensionToRemove)
	{
		return false;
	}
	
	InventoryExtensionSerializer.RemoveExtension(ExtensionToRemove);
	
	RemoveReplicatedSubObject(ExtensionToRemove);
	
	return true;
}

ESaveLoadResult UInventoryComponent::GetSaveData_Implementation(FInventorySaveData& OutSaveData) const
{
	if (!Inventory_PDA)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::GetSaveData_Implementation: Inventory_PDA is null"));
		
		if (OnInventorySaving.IsBound())
		{
			OnInventorySaving.Broadcast(ESaveLoadResult::Failed);
		}
		
		return ESaveLoadResult::Failed;
	}
	
	OutSaveData = FInventorySaveData();
	
	OutSaveData.InventoryPDAPath = Inventory_PDA.GetPathName();

	// Save items data
	for (const auto& Element : InventoryContainer.Entries)
	{
		if (!Element.ItemInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::GetSaveData_Implementation: ItemInstance is null for slot index %d"), Element.SlotIndex);
			
			if (OnInventorySaving.IsBound())
			{
				OnInventorySaving.Broadcast(ESaveLoadResult::Corrupted);
			}
			
			return ESaveLoadResult::Corrupted;
		}
		
		FInventorySlotSaveData SlotData;
		
		Element.ItemInstance->GetSaveData(SlotData.ItemInstanceData);
		
		SlotData.SlotGuid = Element.EntryGuid;
		
		SlotData.SlotIndex = Element.SlotIndex;
		
		OutSaveData.SlotsSaveData.Add(SlotData);
		
	}
	
	// Save extensions data
	for (const auto& Element : InventoryExtensionSerializer.Entries)
	{
		// Check if the extension instance is valid
		if (!Element.ExtensionInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::GetSaveData_Implementation: ExtensionInstance is null"));
			
			if (OnInventorySaving.IsBound())
			{
				OnInventorySaving.Broadcast(ESaveLoadResult::Corrupted);
			}
			
			return ESaveLoadResult::Corrupted;
		}
		
		// Check if the extension should be saved
		if (!Element.ExtensionInstance->bShouldBeSaved)
		{
			continue;
		}

		FSaveExtensionData ExtensionData;
		
		Element.ExtensionInstance->GetSaveData(ExtensionData);
		
		OutSaveData.ExtensionsData.Add(ExtensionData);
	}
	
	if (OnInventorySaving.IsBound())
	{
		OnInventorySaving.Broadcast(ESaveLoadResult::Success);
	}
	
	return ESaveLoadResult::Success;
}

ESaveLoadResult UInventoryComponent::LoadFromSaveData_Implementation(const FInventorySaveData& InSaveData)
{
	if (InSaveData.InventoryPDAPath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::LoadFromSaveData_Implementation: InventoryPDAPath is empty"));
		
		if (OnInventoryLoading.IsBound())
		{
			OnInventoryLoading.Broadcast(ESaveLoadResult::Failed);
		}
		
		return ESaveLoadResult::Failed;
	}
	
	// Clear all items
	if (!InventoryContainer.Entries.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::LoadFromSaveData_Implementation: InventoryContainer is not empty | Clearing all entries"));

		TArray<UItemInstance*> ToRemove;
		for (const auto& Entry : InventoryContainer.Entries)
		{
			if (Entry.ItemInstance) 
			{
				ToRemove.Add(Entry.ItemInstance);
			}
		}
		for (UItemInstance* Item : ToRemove)
		{
			RemoveReplicatedItemAndExtensions(Item);
		}
		InventoryContainer.RemoveAllEntries();
	}
	
	// Clear all extensions
	if (!InventoryExtensionSerializer.Entries.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::LoadFromSaveData_Implementation: InventoryExtensionSerializer is not empty | Clearing all entries"));
		
		Execute_RemoveAllExtensions(this);
	}
	
	UPDA_Inventory* LoadedPDA = nullptr;
	
	if (UItemFactorySubsystem* ItemFactory = GetWorld()->GetGameInstance()->GetSubsystem<UItemFactorySubsystem>())
	{
		LoadedPDA = ItemFactory->CreatePDAFromString_Inventory(InSaveData.InventoryPDAPath);
		
		if (!LoadedPDA)
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::LoadFromSaveData_Implementation: Failed to load PDA from %s"), *InSaveData.InventoryPDAPath);
			
			if (OnInventoryLoading.IsBound())
			{
				OnInventoryLoading.Broadcast(ESaveLoadResult::Failed);
			}
			
			return ESaveLoadResult::Failed;
		}
		
		/*
		 * Local variables to hold extensions and items to load.
		 * This is done to ensure that if any part of the loading process fails, we can handle it gracefully without leaving the inventory in a partially loaded state.
		 */
		
		TArray<UExtension*> ExtensionsToLoad;
		
		TArray<UExtension*> DefaultExtensionsToLoad;
		
		TArray<FInventoryEntry> EntriesToLoad;
		
		
		// Load extensions data
		for (const auto& ExtensionData : InSaveData.ExtensionsData)
		{
			if (!ExtensionData.ExtensionClass.IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::LoadFromSaveData_Implementation: ExtensionClass is invalid"));
				
				if (OnInventoryLoading.IsBound())
				{
					OnInventoryLoading.Broadcast(ESaveLoadResult::Corrupted);
				}
				
				return ESaveLoadResult::Corrupted;
			}
			
			UClass* ExtensionClass = ExtensionData.ExtensionClass.TryLoadClass<UExtension>();
			
			if (!ExtensionClass)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to load Extension Class: %s"), *ExtensionData.ExtensionClass.ToString());
				
				if (OnInventoryLoading.IsBound())
				{
					OnInventoryLoading.Broadcast(ESaveLoadResult::Corrupted);
				}
				
				return ESaveLoadResult::Corrupted;
			}
			
			UExtension* NewExtension = NewObject<UExtension>(this, ExtensionClass);
			
			NewExtension->LoadFromSaveData(ExtensionData);
			
			ExtensionsToLoad.Add(NewExtension);
		}
		
		// Load default extensions from the Inventory_PDA
		for (const auto& DefaultExtension : LoadedPDA->DefaultExtensions)
		{
			if (!DefaultExtension)
			{
				UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::LoadFromSaveData_Implementation: DefaultExtension is null"));
				
				if (OnInventoryLoading.IsBound())
				{
					OnInventoryLoading.Broadcast(ESaveLoadResult::Corrupted);
				}
				
				return ESaveLoadResult::Corrupted;
			}
			
			UInventoryExtension* NewExtension = DuplicateObject<UInventoryExtension>(DefaultExtension, this);
			
			if (!NewExtension)
			{
				UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::LoadFromSaveData_Implementation: Failed to duplicate DefaultExtension"));
				
				if (OnInventoryLoading.IsBound())
				{
					OnInventoryLoading.Broadcast(ESaveLoadResult::Corrupted);
				}
				
				return ESaveLoadResult::Corrupted;
			}
			
			DefaultExtensionsToLoad.Add(NewExtension);
		}
		
		// Load items data & slots
		for (const auto& SlotData : InSaveData.SlotsSaveData)
		{
			if (!SlotData.ItemInstanceData.ItemPDAPath.IsEmpty())
			{
				UPDA_Item* ItemPDA = ItemFactory->CreatePDAFromString_Item(SlotData.ItemInstanceData.ItemPDAPath);
				
				if (!ItemPDA)
				{
					UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::LoadFromSaveData_Implementation: Failed to create PDA from string %s"), *SlotData.ItemInstanceData.ItemPDAPath);
					
					if (OnInventoryLoading.IsBound())
					{
						OnInventoryLoading.Broadcast(ESaveLoadResult::Corrupted);
					}
					
					return ESaveLoadResult::Corrupted;
				}
				
				UItemInstance* NewItemInstance = ItemFactory->CreateItemInstanceFromPDA(ItemPDA, this);
				
				if (!NewItemInstance)
				{
					UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::LoadFromSaveData_Implementation: Failed to create ItemInstance from PDA path %s"), *SlotData.ItemInstanceData.ItemPDAPath);
					
					if (OnInventoryLoading.IsBound())
					{
						OnInventoryLoading.Broadcast(ESaveLoadResult::Corrupted);
					}
					
					return ESaveLoadResult::Corrupted;
					
				}

				NewItemInstance->LoadFromSaveData(SlotData.ItemInstanceData);
				
				FInventoryEntry NewEntry(NewItemInstance, SlotData.SlotIndex, SlotData.SlotGuid);
				EntriesToLoad.Add(NewEntry);
			}
		}
		
		Inventory_PDA = LoadedPDA;
		
		if (!ExtensionsToLoad.IsEmpty())
		{
			for (UExtension* Extension : ExtensionsToLoad)
			{
				Execute_AddExtensionInstance(this, Extension);
			}
		}
		
		if (!DefaultExtensionsToLoad.IsEmpty())
		{
			for (UExtension* Extension : DefaultExtensionsToLoad)
			{
				if (Execute_FindExtension(this, Extension->GetClass()))
				{
					continue;
				}
				
				Execute_AddExtensionInstance(this, Extension);
			}
		}
		
		if (!EntriesToLoad.IsEmpty())
		{
			for (const auto& Entry : EntriesToLoad)
			{
				InventoryContainer.AddEntry(Entry.ItemInstance, Entry.SlotIndex, Entry.EntryGuid);
				ReplicateItemAndExtensions(Entry.ItemInstance);
			}
		}
		
		if (OnInventoryLoading.IsBound())
		{
			OnInventoryLoading.Broadcast(ESaveLoadResult::Success);
		}
		
		return ESaveLoadResult::Success;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::LoadFromSaveData_Implementation: ItemFactorySubsystem not found"));
		
		if (OnInventoryLoading.IsBound())
		{
			OnInventoryLoading.Broadcast(ESaveLoadResult::Failed);
		}
		
		return ESaveLoadResult::Failed;
	}
	
}
