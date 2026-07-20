// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "Inventory/InventoryComponent.h"

#include "Engine/ActorChannel.h"

#include "Inventory/InventoryExtension/InventoryExtension.h"
#include "Inventory/InventoryPrimaryDataAssets/PDA_Inventory.h"

#include "Net/UnrealNetwork.h"

#include "Wrappers/ExtensionContainer.h"


UInventoryComponent::UInventoryComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;
	
	
	InventoryExtensionContainer = CreateDefaultSubobject<UInventoryExtensionContainer>(TEXT("InventoryExtensionContainer"));
	
	SetIsReplicatedByDefault(true);
	
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (Inventory_PDA && InventoryExtensionContainer)
	{
		for (UInventoryExtension* DefaultExt : Inventory_PDA->DefaultExtensions)
		{
			if (DefaultExt)
			{
				UInventoryExtension* CopiedExt = DuplicateObject<UInventoryExtension>(DefaultExt, InventoryExtensionContainer);
				
				InventoryExtensionContainer->AddExtensionInstance(CopiedExt);
			}
		}
	}

	
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

bool UInventoryComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	if (InventoryExtensionContainer && InventoryExtensionContainer->IsValidLowLevel())
	{
		WroteSomething |= Channel->ReplicateSubobject(InventoryExtensionContainer, *Bunch, *RepFlags);
		
		for (const FInventoryExtensionEntry& Entry : InventoryExtensionContainer->GetExtensionList().Entries)
		{
			if (Entry.ExtensionInstance)
			{
				WroteSomething |= Channel->ReplicateSubobject(Entry.ExtensionInstance.Get(), *Bunch, *RepFlags);
			}
		}
	}
	
	for (const FInventoryEntry& Entry : InventoryContainer.Entries)
	{
		if (Entry.ItemInstance && Entry.ItemInstance->IsValidLowLevel())
		{
			WroteSomething |= Channel->ReplicateSubobject(Entry.ItemInstance, *Bunch, *RepFlags);
			
			if (UItemExtensionContainer* ItemExtContainer = Entry.ItemInstance->GetExtensionContainer())
			{
				WroteSomething |= Channel->ReplicateSubobject(ItemExtContainer, *Bunch, *RepFlags);
				
				for (const FItemExtensionEntry& ExtEntry : ItemExtContainer->GetExtensionList().Entries)
				{
					if (ExtEntry.ExtensionInstance)
					{
						WroteSomething |= Channel->ReplicateSubobject(ExtEntry.ExtensionInstance.Get(), *Bunch, *RepFlags);
					}
				}
			}
		}
	}

	return WroteSomething;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UInventoryComponent, InventoryContainer);
	DOREPLIFETIME(UInventoryComponent, InventoryExtensionContainer); 
	
}
