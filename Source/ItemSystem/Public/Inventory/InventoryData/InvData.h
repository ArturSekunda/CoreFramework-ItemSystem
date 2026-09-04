#pragma once

#include "CoreMinimal.h"

#include "Net/Serialization/FastArraySerializer.h"

#include "InvData.generated.h"

class UItemInstance;

USTRUCT(BlueprintType)
struct ITEMSYSTEM_API FInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<UItemInstance> ItemInstance;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FGuid EntryGuid;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	int32 SlotIndex;
	
	FInventoryEntry() : ItemInstance(nullptr), EntryGuid(FGuid::NewGuid()), SlotIndex(-1) {}
	FInventoryEntry(UItemInstance* InItem, int32 InSlotIndex, FGuid InEntryGuid = FGuid::NewGuid()) : ItemInstance(InItem), EntryGuid(InEntryGuid), SlotIndex(InSlotIndex) { }
	
	void PreReplicatedRemove(const struct FInventoryContainerList& InArraySerializer) { }
	void PostReplicatedAdd(const struct FInventoryContainerList& InArraySerializer) { }
	void PostReplicatedChange(const struct FInventoryContainerList& InArraySerializer) { }
};

USTRUCT(BlueprintType)
struct ITEMSYSTEM_API FInventoryContainerList : public FFastArraySerializer
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<FInventoryEntry> Entries;
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryEntry, FInventoryContainerList>(Entries, DeltaParms, *this);
	}
	
	void AddEntry(UItemInstance* NewItem, int32 SlotIndex, FGuid EntryGuid = FGuid::NewGuid())
	{
		FInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.ItemInstance = NewItem;
		NewEntry.SlotIndex = SlotIndex;
		NewEntry.EntryGuid = EntryGuid;
		MarkItemDirty(NewEntry);
	}

	void RemoveEntry(UItemInstance* ItemToRemove)
	{
		for (auto It = Entries.CreateIterator(); It; ++It)
		{
			if (It->ItemInstance == ItemToRemove)
			{
				It.RemoveCurrent();
				MarkArrayDirty();
				break;
			}
		}
	}
};

template<>
struct TStructOpsTypeTraits<FInventoryContainerList> : public TStructOpsTypeTraitsBase2<FInventoryContainerList>
{
	enum { WithNetDeltaSerializer = true };
};
