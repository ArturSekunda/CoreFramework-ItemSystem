#pragma once

#include "CoreMinimal.h"

#include "Net/Serialization/FastArraySerializer.h"

#include "InvData.generated.h"

class UItemInstance;

UENUM(BlueprintType)
enum class EInventorySaveLoadResult : uint8
{
	Success UMETA(DisplayName = "Success"),
	Corrupted UMETA(DisplayName = "Corrupted"),
	Failed UMETA(DisplayName = "Failed"),
};

USTRUCT(BlueprintType)
struct ITEMSYSTEM_API FInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UItemInstance> ItemInstance;
	
	UPROPERTY(BlueprintReadOnly)
	int32 SlotIndex = -1;
	
	FInventoryEntry() : ItemInstance(nullptr), SlotIndex(-1) {}
	FInventoryEntry(UItemInstance* InItem, int32 InSlotIndex) : ItemInstance(InItem), SlotIndex(InSlotIndex) { }
	
	void PreReplicatedRemove(const struct FInventoryContainerList& InArraySerializer) { }
	void PostReplicatedAdd(const struct FInventoryContainerList& InArraySerializer) { }
	void PostReplicatedChange(const struct FInventoryContainerList& InArraySerializer) { }
};

USTRUCT(BlueprintType)
struct ITEMSYSTEM_API FInventoryContainerList : public FFastArraySerializer
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FInventoryEntry> Entries;
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryEntry, FInventoryContainerList>(Entries, DeltaParms, *this);
	}
	
	void AddEntry(UItemInstance* NewItem, int32 SlotIndex)
	{
		FInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.ItemInstance = NewItem;
		NewEntry.SlotIndex = SlotIndex;
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
