// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "Items/ItemsActors/ItemActor.h"

#include "Engine/ActorChannel.h"

#include "Items/ItemsInstances/ItemInstance.h"

#include "Net/UnrealNetwork.h"

#include "Wrappers/ExtensionContainer.h"
#include "Wrappers/WrapperData.h"


// Sets default values
AItemActor::AItemActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	AActor::SetReplicateMovement(true); 
}

// Called when the game starts or when spawned
void AItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AItemActor, ItemInstance);
}

bool AItemActor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (IsValid(ItemInstance))
	{
		WroteSomething |= Channel->ReplicateSubobject(ItemInstance.Get(), *Bunch, *RepFlags);

		if (UItemExtensionContainer* Container = ItemInstance->GetExtensionContainer())
		{
			WroteSomething |= Channel->ReplicateSubobject(Container, *Bunch, *RepFlags);

			for (const FItemExtensionEntry& Entry : Container->GetExtensionList().Entries)
			{
				if (Entry.ExtensionInstance)
				{
					WroteSomething |= Channel->ReplicateSubobject(Entry.ExtensionInstance.Get(), *Bunch, *RepFlags);
				}
			}
		}
	}

	return WroteSomething;
}

UItemInstance* AItemActor::GetItemInstance() const
{
	if (IsValid(ItemInstance))
	{
		return ItemInstance;
	}
	
	return nullptr;
}

void AItemActor::InitializeItem(UItemInstance* InItemInstance)
{
	ItemInstance = InItemInstance;
}

