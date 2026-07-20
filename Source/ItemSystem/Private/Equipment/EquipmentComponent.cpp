// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "Equipment/EquipmentComponent.h"

#include "Engine/ActorChannel.h"

#include "Equipment/EquipmentExtension/EquipmentExtension.h"
#include "Equipment/EquipmentPrimaryDataAssets/PDA_Equipment.h"

#include "Net/UnrealNetwork.h"

#include "Wrappers/ExtensionContainer.h"


// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{

	PrimaryComponentTick.bCanEverTick = false;
	
	EquipmentExtensionContainer = CreateDefaultSubobject<UEquipmentExtensionContainer>(TEXT("EquipmentExtensionContainer"));
	
	SetIsReplicatedByDefault(true);
	
}

void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Equipment_PDA && EquipmentExtensionContainer)
	{
		for (UEquipmentExtension* DefaultExt : Equipment_PDA->DefaultExtensions)
		{
			if (DefaultExt)
			{
				UEquipmentExtension* CopiedExt = DuplicateObject<UEquipmentExtension>(DefaultExt, EquipmentExtensionContainer);
				
				EquipmentExtensionContainer->AddExtensionInstance(CopiedExt);
			}
		}
	}
	
}


// Called every frame
void UEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UEquipmentComponent, EquipmentExtensionContainer);
}

bool UEquipmentComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	if (EquipmentExtensionContainer && EquipmentExtensionContainer->IsValidLowLevel())
	{
		WroteSomething |= Channel->ReplicateSubobject(EquipmentExtensionContainer, *Bunch, *RepFlags);
		
		for (const FEquipmentExtensionEntry& Entry : EquipmentExtensionContainer->GetExtensionList().Entries)
		{
			if (Entry.ExtensionInstance)
			{
				WroteSomething |= Channel->ReplicateSubobject(Entry.ExtensionInstance.Get(), *Bunch, *RepFlags);
			}
		}
	}

	return WroteSomething;
}

