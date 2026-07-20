// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.


#include "Items/ItemsInstances/ItemInstance.h"

#include "Items/ItemsPrimaryDataAssets/PDA_Item.h"
#include "Wrappers/ExtensionContainer.h"

#include "Net/UnrealNetwork.h"

UItemInstance::UItemInstance()
{
	ExtensionContainer = CreateDefaultSubobject<UItemExtensionContainer>(TEXT("ExtensionContainer"));
}

void UItemInstance::InitializeFromPDA(UPDA_Item* NewPDA, FGuid NewGuid)
{
	if (!NewPDA) return;
	
	Item_PDA = NewPDA;
	
	Guid = NewGuid;
	
	if (ExtensionContainer)
	{
		for (UItemExtension* DefaultExt : Item_PDA->DefaultExtensions)
		{
			if (DefaultExt)
			{
				UItemExtension* CopiedExt = DuplicateObject<UItemExtension>(DefaultExt, ExtensionContainer);
				
				ExtensionContainer->AddExtensionInstance(CopiedExt);
			}
		}
	}
}

void UItemInstance::InitializeFromSaveFile(UPDA_Item* NewPDA, FGuid NewGuid, FEditableItemBaseData NewEditableBaseData, UItemExtensionContainer* NewExtensionContainer)
{
	if (!NewPDA) return;
	
	Item_PDA = NewPDA;
	
	Guid = NewGuid;
	
	EditableBaseData = NewEditableBaseData;
	
	if (ExtensionContainer && NewExtensionContainer)
	{
		ExtensionContainer->RemoveAllExtensions();
		
		for (UItemExtension* SavedExt : NewExtensionContainer->GetExtensions_BP())
		{
			if (SavedExt)
			{
				UItemExtension* CopiedExt = DuplicateObject<UItemExtension>(SavedExt, ExtensionContainer);
				ExtensionContainer->AddExtensionInstance(CopiedExt);
			}
		}
	}
	
}

void UItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UItemInstance, EditableBaseData);
	DOREPLIFETIME(UItemInstance, Item_PDA);
	DOREPLIFETIME(UItemInstance, ExtensionContainer); 
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
	UE_LOG(LogTemp, Warning, TEXT("ExtensionContainer: %s"), ExtensionContainer ? *ExtensionContainer->GetName() : TEXT("None"));
	if (ExtensionContainer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Extensions:"));
		for (const FItemExtensionEntry& Entry : ExtensionContainer->GetExtensionList().Entries)
		{
			UE_LOG(LogTemp, Warning, TEXT(" - %s"), Entry.ExtensionInstance ?*Entry.ExtensionInstance->GetName() : TEXT("None"));
		}
	}
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
