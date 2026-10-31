// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "Equipment/EquipmentComponent.h"

#include "Equipment/EquipmentExtension/EquipmentExtension.h"
#include "Equipment/EquipmentPrimaryDataAssets/PDA_Equipment.h"

#include "Net/UnrealNetwork.h"

#include "Interface/ExtensionContainer.h"

#include "Subsystems/ItemFactorySubsystem.h"


// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{

	PrimaryComponentTick.bCanEverTick = false;
	
	bReplicateUsingRegisteredSubObjectList = true;
	SetIsReplicatedByDefault(true);
	
}

void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Prepare default extensions for the Equipment Component.
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
	
	DOREPLIFETIME(UEquipmentComponent, EquipmentExtensionSerializer);
}

void UEquipmentComponent::PrepareDefaultExtensions_Implementation()
{
	for (TObjectPtr<UEquipmentExtension> DefaultExtension : Equipment_PDA->DefaultExtensions)
	{
		if (DefaultExtension)
		{
			UEquipmentExtension* CopiedExt = DuplicateObject<UEquipmentExtension>(DefaultExtension, this);
			
			if (!CopiedExt)
			{
				UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::PrepareDefaultExtensions_Implementation: Failed to duplicate extension %s"), *DefaultExtension->GetName());
				continue;
			}
			
			Execute_AddExtensionInstance(this, CopiedExt);
		}
	}
}

bool UEquipmentComponent::AddExtensionInstance_Implementation(UExtension* NewExtension)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::AddExtensionInstance_Implementation: Only the server can add extensions!"));
		return false;
	}
	
	if (!NewExtension)
	{
		return false;
	}

	for (const FEquipmentExtensionEntry& Entry : EquipmentExtensionSerializer.Entries)
	{
		if (Entry.ExtensionInstance == NewExtension) return false;
	}
	
	UEquipmentExtension* EquipmentExt = Cast<UEquipmentExtension>(NewExtension);
	if (!EquipmentExt)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::AddExtensionInstance_Implementation: NewExtension is not a UEquipmentExtension!"));
		return false;
	}
	
	EquipmentExt->Rename(nullptr, this);
	
	EquipmentExt->SetOwningEquipmentComponent(this);

	EquipmentExt->InitializeExtension();
	
	AddReplicatedSubObject(EquipmentExt);
	
	EquipmentExtensionSerializer.AddExtension(EquipmentExt);

	return true;
}

UExtension* UEquipmentComponent::AddNewExtensionByClass_Implementation(TSubclassOf<UExtension> ExtensionClass)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::AddNewExtensionByClass_Implementation: Only the server can add extensions!"));
		return nullptr;
	}
	
	if (!ExtensionClass)
	{
		return nullptr;
	}

	EquipmentExtensionSerializer.RemoveInvalidEntries();

	if (UExtension* Existing = Execute_FindExtension(this, ExtensionClass))
	{
		return Existing;
	}

	UEquipmentExtension* CreatedExtension = NewObject<UEquipmentExtension>(this, ExtensionClass);
	
	CreatedExtension->SetOwningEquipmentComponent(this);
	
	CreatedExtension->InitializeExtension();
	
	AddReplicatedSubObject(CreatedExtension);
	
	EquipmentExtensionSerializer.AddExtension(CreatedExtension);

	return CreatedExtension;
}

UExtension* UEquipmentComponent::FindExtension_Implementation(TSubclassOf<UExtension> ExtensionClass) const
{
	
	if (!ExtensionClass)
	{
		return nullptr;
	}

	for (const FEquipmentExtensionEntry& Entry : EquipmentExtensionSerializer.Entries)
	{
		if (IsValid(Entry.ExtensionInstance) && Entry.ExtensionInstance->IsA(ExtensionClass))
		{
			return Entry.ExtensionInstance;
		}
	}
	
	return nullptr;
}

TArray<UExtension*> UEquipmentComponent::GetExtensions_BP_Implementation() const
{
	
	TArray<UExtension*> OutExtensions;
	for (const FEquipmentExtensionEntry& Entry : EquipmentExtensionSerializer.Entries)
	{
		if (Entry.ExtensionInstance)
		{
			OutExtensions.Add(Entry.ExtensionInstance);
		}
	}
	return OutExtensions;
}

void UEquipmentComponent::RemoveAllExtensions_Implementation()
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveAllExtensions_Implementation called on non-authority role. This may lead to unexpected behavior."));
		return;
	}

	for (const auto& Element : EquipmentExtensionSerializer.Entries)
	{
		if (Element.ExtensionInstance)
		{
			RemoveReplicatedSubObject(Element.ExtensionInstance);
		}
	}

	EquipmentExtensionSerializer.Entries.Empty();
	EquipmentExtensionSerializer.MarkArrayDirty();
}

bool UEquipmentComponent::RemoveExtension_Implementation(UExtension* ExtensionToRemove)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveExtension_Implementation called on non-authority role. This may lead to unexpected behavior."));
		return false;
	}
	
	if (!ExtensionToRemove)
	{
		return false;
	}
	
	RemoveReplicatedSubObject(ExtensionToRemove);

	EquipmentExtensionSerializer.RemoveExtension(ExtensionToRemove);
	
	return true;
}

ESaveLoadResult UEquipmentComponent::GetSaveData_Implementation(FEquipmentSaveData& OutSaveData) const
{
	OutSaveData = FEquipmentSaveData();

	if (!Equipment_PDA)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::GetSaveData: Equipment_PDA is null"));
		
		if (OnEquipmentSaving.IsBound())
		{
			OnEquipmentSaving.Broadcast(ESaveLoadResult::Failed);
		}
		
		return ESaveLoadResult::Failed;
	}

	OutSaveData.EquipmentPDAPath = Equipment_PDA.GetPathName();

	for (const auto& Element : EquipmentExtensionSerializer.Entries)
	{
		if (!Element.ExtensionInstance || !Element.ExtensionInstance->bShouldBeSaved)
		{
			continue;
		}

		FSaveExtensionData ExtensionData;
		Element.ExtensionInstance->GetSaveData(ExtensionData);
		OutSaveData.ExtensionsData.Add(ExtensionData);
	}
	
	if (OnEquipmentSaving.IsBound())
	{
		OnEquipmentSaving.Broadcast(ESaveLoadResult::Success);
	}

	return ESaveLoadResult::Success;
}

ESaveLoadResult UEquipmentComponent::LoadFromSaveData_Implementation(const FEquipmentSaveData& InSaveData)
{
    if (InSaveData.EquipmentPDAPath.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::LoadFromSaveData: EquipmentPDAPath is empty"));
    	
    	if (OnEquipmentLoading.IsBound())
    	{
    		OnEquipmentLoading.Broadcast(ESaveLoadResult::Failed);
    	}
    	
        return ESaveLoadResult::Failed;
    }

    UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	
    UItemFactorySubsystem* ItemFactory = GI ? GI->GetSubsystem<UItemFactorySubsystem>() : nullptr;
    if (!ItemFactory)
    {
        UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::LoadFromSaveData: ItemFactorySubsystem not found"));
    	
    	if (OnEquipmentLoading.IsBound())
    	{
    		OnEquipmentLoading.Broadcast(ESaveLoadResult::Failed);
    	}
    	
        return ESaveLoadResult::Failed;
    }

    UPDA_Equipment* LoadedPDA = ItemFactory->CreatePDAFromString_Equipment(InSaveData.EquipmentPDAPath);
	
    if (!LoadedPDA)
    {
        UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::LoadFromSaveData: Failed to load PDA from %s"), *InSaveData.EquipmentPDAPath);
    	
    	if (OnEquipmentLoading.IsBound())
    	{
    		OnEquipmentLoading.Broadcast(ESaveLoadResult::Failed);
    	}
    	
        return ESaveLoadResult::Failed;
    }
	

	TArray<UExtension*> ExtensionsToLoad;
    
    for (UEquipmentExtension* DefaultExtension : LoadedPDA->DefaultExtensions)
    {
        if (!DefaultExtension)
        {
            UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::LoadFromSaveData: DefaultExtension is null"));
            continue;
        }

        UEquipmentExtension* NewExtension = DuplicateObject<UEquipmentExtension>(DefaultExtension, this);
        if (!NewExtension)
        {
            UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::LoadFromSaveData: DuplicateObject failed"));
        	
        	if (OnEquipmentLoading.IsBound())
			{
				OnEquipmentLoading.Broadcast(ESaveLoadResult::Corrupted);
			}
        	
            return ESaveLoadResult::Corrupted;
        }

        if (NewExtension->bShouldBeSaved)
        {
            for (const FSaveExtensionData& Saved : InSaveData.ExtensionsData)
            {
                if (Saved.ExtensionClass.TryLoadClass<UExtension>() == NewExtension->GetClass())
                {
                    NewExtension->LoadFromSaveData(Saved);
                    break;
                }
            }
        }

        ExtensionsToLoad.Add(NewExtension);
    }
	
	Equipment_PDA = LoadedPDA;
	
	// Clear all existing extensions before loading new ones
	if (!EquipmentExtensionSerializer.Entries.IsEmpty())
	{
		Execute_RemoveAllExtensions(this);
	}
	
	if (!ExtensionsToLoad.IsEmpty())
	{
		for (UExtension* Extension : ExtensionsToLoad)
		{
			Execute_AddExtensionInstance(this, Extension);
		}
	}
	
	if (OnEquipmentLoading.IsBound())
	{
		OnEquipmentLoading.Broadcast(ESaveLoadResult::Success);
	}

    return ESaveLoadResult::Success;
}

