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

void UInventoryComponent::ReplicateItemAndExtensions(TObjectPtr<UItemInstance>& InItemInstance)
{
	AddReplicatedSubObject(InItemInstance);
	
	for (UExtension* Ext : InItemInstance->Execute_GetExtensions_BP(InItemInstance))
	{
		if (Ext)
		{
			AddReplicatedSubObject(Ext);
		}
	}
	
}

void UInventoryComponent::RemoveReplicatedItemAndExtensions(TObjectPtr<UItemInstance>& InItemInstance)
{
	RemoveReplicatedSubObject(InItemInstance);
	
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
	
	if (UItemFactorySubsystem* ItemFactory = GetWorld()->GetGameInstance()->GetSubsystem<UItemFactorySubsystem>())
	{
		Inventory_PDA = ItemFactory->CreatePDAFromString_Inventory(InSaveData.InventoryPDAPath);
		
		if (!Inventory_PDA)
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::LoadFromSaveData_Implementation: Failed to load InventoryPDA from path %s"), *InSaveData.InventoryPDAPath);
			
			if (OnInventoryLoading.IsBound())
			{
				OnInventoryLoading.Broadcast(ESaveLoadResult::Failed);
			}
			
			return ESaveLoadResult::Failed;
		}
		
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
			
			Execute_AddExtensionInstance(this, NewExtension);
		}
		
		// Load default extensions from the Inventory_PDA
		for (const auto& DefaultExtension : Inventory_PDA->DefaultExtensions)
		{
			if (!DefaultExtension)
			{
				UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::LoadFromSaveData_Implementation: DefaultExtension is null"));
				
				if (OnInventoryLoading.IsBound())
				{
					OnInventoryLoading.Broadcast(ESaveLoadResult::Corrupted);
				
					return ESaveLoadResult::Corrupted;
				}
				
			}
			
			UInventoryExtension* NewExtension = DuplicateObject<UInventoryExtension>(DefaultExtension, this);
			
			if (!NewExtension)
			{
				UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::LoadFromSaveData_Implementation: Failed to duplicate DefaultExtension"));
				
				if (OnInventoryLoading.IsBound())
				{
					OnInventoryLoading.Broadcast(ESaveLoadResult::Corrupted);
				
					return ESaveLoadResult::Corrupted;
				}
			}
			
			Execute_AddExtensionInstance(this, NewExtension);
		}
		
		// Load items data & slots
		for (const auto& SlotData : InSaveData.SlotsSaveData)
		{
			if (!SlotData.ItemInstanceData.ItemPDAPath.IsEmpty())
			{
				
				UItemInstance* NewItemInstance = ItemFactory->CreateItemInstanceFromPDAPathName(SlotData.ItemInstanceData.ItemPDAPath, this);
				
				if (!NewItemInstance)
				{
					UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::LoadFromSaveData_Implementation: Failed to create ItemInstance from PDA path %s"), *SlotData.ItemInstanceData.ItemPDAPath);
					
					if (OnInventoryLoading.IsBound())
					{
						OnInventoryLoading.Broadcast(ESaveLoadResult::Corrupted);
				
						return ESaveLoadResult::Corrupted;
					}
					
				}

				NewItemInstance->LoadFromSaveData(SlotData.ItemInstanceData);
				
				InventoryContainer.AddEntry(NewItemInstance, SlotData.SlotIndex, SlotData.SlotGuid);
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
