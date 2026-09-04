// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "Items/ItemsActors/ItemActor.h"

#include "Items/ItemsInstances/ItemInstance.h"

#include "Net/UnrealNetwork.h"


// Sets default values
AItemActor::AItemActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	bReplicateUsingRegisteredSubObjectList = true;
	AActor::SetReplicateMovement(true); 
}

// Called when the game starts or when spawned
void AItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItemActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority() && IsValid(ItemInstance))
	{
		RemoveReplicatedSubObject(ItemInstance);
		
		for (const auto& Element : ItemInstance->Execute_GetExtensions_BP(ItemInstance))
		{
			if (IsValid(Element))
			{
				RemoveReplicatedSubObject(Element);
			}
		}
	}
	
	Super::EndPlay(EndPlayReason);
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

UItemInstance* AItemActor::GetItemInstance() const
{
	if (IsValid(ItemInstance))
	{
		return ItemInstance;
	}
	
	return nullptr;
}

void AItemActor::InitializeItemActor(UItemInstance* InItemInstance)
{
	if (!HasAuthority() || !IsValid(InItemInstance))
	{
		return;
	}
	
	AddReplicatedSubObject(InItemInstance);

	for (const auto& Element : InItemInstance->Execute_GetExtensions_BP(InItemInstance))
	{
		if (IsValid(Element))
		{
			AddReplicatedSubObject(Element);
		}
	}
	
	ItemInstance = InItemInstance;
}

