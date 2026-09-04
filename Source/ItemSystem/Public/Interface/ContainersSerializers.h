#pragma once

#include "CoreMinimal.h"

#include "Equipment/EquipmentExtension/EquipmentExtension.h"

#include "Inventory/InventoryExtension/InventoryExtension.h"

#include "Items/ItemsExtensions/ItemExtension.h"

#include "Net/Serialization/FastArraySerializer.h"

#include "ContainersSerializers.generated.h"

USTRUCT(BlueprintType)
struct ITEMSYSTEM_API FItemExtensionEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly)
	TObjectPtr<UItemExtension> ExtensionInstance;
	
	FItemExtensionEntry() : ExtensionInstance(nullptr) {}
	FItemExtensionEntry(UItemExtension* InExtension) : ExtensionInstance(InExtension) { }
	
	void PreReplicatedRemove(const struct FItemExtensionContainerList& InArraySerializer) { }
	void PostReplicatedAdd(const struct FItemExtensionContainerList& InArraySerializer)
	{
		if (ExtensionInstance)
		{
			ExtensionInstance->InitializeExtension(); 
		}
		
	}
	void PostReplicatedChange(const struct FItemExtensionContainerList& InArraySerializer) { }
};

USTRUCT(BlueprintType)
struct ITEMSYSTEM_API FItemExtensionContainerList : public FFastArraySerializer
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<FItemExtensionEntry> Entries;
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FItemExtensionEntry, FItemExtensionContainerList>(Entries, DeltaParms, *this);
	}
	
	void AddExtension(UItemExtension* NewExt)
	{
		FItemExtensionEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.ExtensionInstance = NewExt;
		MarkItemDirty(NewEntry);
	}

	void RemoveExtension(UExtension* ExtToRemove)
	{
		for (auto It = Entries.CreateIterator(); It; ++It)
		{
			if (It->ExtensionInstance == ExtToRemove)
			{
				It.RemoveCurrent();
				MarkArrayDirty();
				break;
			}
		}
	}
	
	void RemoveInvalidEntries()
	{
		for (int32 i = Entries.Num() - 1; i >= 0; --i)
		{
			if (!Entries[i].ExtensionInstance || !IsValid(Entries[i].ExtensionInstance))
			{
				Entries.RemoveAt(i);
				MarkArrayDirty();
			}
		}
	}
};

template<>
struct TStructOpsTypeTraits<FItemExtensionContainerList> : public TStructOpsTypeTraitsBase2<FItemExtensionContainerList>
{
	enum { WithNetDeltaSerializer = true };
};

USTRUCT(BlueprintType)
struct ITEMSYSTEM_API FInventoryExtensionEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly)
	TObjectPtr<UInventoryExtension> ExtensionInstance;
	
	FInventoryExtensionEntry() : ExtensionInstance(nullptr) {}
	FInventoryExtensionEntry(UInventoryExtension* InExtension) : ExtensionInstance(InExtension) { }
	
	void PreReplicatedRemove(const struct FInventoryExtensionContainerList& InArraySerializer){ }
	void PostReplicatedAdd(const struct FInventoryExtensionContainerList& InArraySerializer)
	{
		if (ExtensionInstance)
		{
			ExtensionInstance->InitializeExtension(); 
		}
	}
	void PostReplicatedChange(const struct FInventoryExtensionContainerList& InArraySerializer) { }
};

USTRUCT(BlueprintType)
struct ITEMSYSTEM_API FInventoryExtensionContainerList : public FFastArraySerializer
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<FInventoryExtensionEntry> Entries;
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryExtensionEntry, FInventoryExtensionContainerList>(Entries, DeltaParms, *this);
	}
	
	void AddExtension(UInventoryExtension* NewExt)
	{
		FInventoryExtensionEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.ExtensionInstance = NewExt;
		MarkItemDirty(NewEntry);
	}

	void RemoveExtension(UExtension* ExtToRemove)
	{
		for (auto It = Entries.CreateIterator(); It; ++It)
		{
			if (It->ExtensionInstance == ExtToRemove)
			{
				It.RemoveCurrent();
				MarkArrayDirty();
				break;
			}
		}
	}
	
	void RemoveInvalidEntries()
	{
		for (int32 i = Entries.Num() - 1; i >= 0; --i)
		{
			if (!Entries[i].ExtensionInstance || !IsValid(Entries[i].ExtensionInstance))
			{
				Entries.RemoveAt(i);
				MarkArrayDirty();
			}
		}
	}
};

template<>
struct TStructOpsTypeTraits<FInventoryExtensionContainerList> : public TStructOpsTypeTraitsBase2<FInventoryExtensionContainerList>
{
	enum { WithNetDeltaSerializer = true };
};

USTRUCT(BlueprintType)
struct ITEMSYSTEM_API FEquipmentExtensionEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly)
	TObjectPtr<UEquipmentExtension> ExtensionInstance;
	
	FEquipmentExtensionEntry() : ExtensionInstance(nullptr) {}
	FEquipmentExtensionEntry(UEquipmentExtension* InExtension) : ExtensionInstance(InExtension) { }
	
	void PreReplicatedRemove(const struct FEquipmentExtensionContainerList& InArraySerializer) { }
	void PostReplicatedAdd(const struct FEquipmentExtensionContainerList& InArraySerializer)
	{
		if (ExtensionInstance)
		{
			ExtensionInstance->InitializeExtension(); 
		}
	}
	void PostReplicatedChange(const struct FEquipmentExtensionContainerList& InArraySerializer) { }
};

USTRUCT(BlueprintType)
struct ITEMSYSTEM_API FEquipmentExtensionContainerList : public FFastArraySerializer
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<FEquipmentExtensionEntry> Entries;
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FEquipmentExtensionEntry, FEquipmentExtensionContainerList>(Entries, DeltaParms, *this);
	}
	
	void AddExtension(UEquipmentExtension* NewExt)
	{
		FEquipmentExtensionEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.ExtensionInstance = NewExt;
		MarkItemDirty(NewEntry);
	}

	void RemoveExtension(UExtension* ExtToRemove)
	{
		for (auto It = Entries.CreateIterator(); It; ++It)
		{
			if (It->ExtensionInstance == ExtToRemove)
			{
				It.RemoveCurrent();
				MarkArrayDirty();
				break;
			}
		}
	}
	
	void RemoveInvalidEntries()
	{
		for (int32 i = Entries.Num() - 1; i >= 0; --i)
		{
			if (!Entries[i].ExtensionInstance || !IsValid(Entries[i].ExtensionInstance))
			{
				Entries.RemoveAt(i);
				MarkArrayDirty();
			}
		}
	}
};

template<>
struct TStructOpsTypeTraits<FEquipmentExtensionContainerList> : public TStructOpsTypeTraitsBase2<FEquipmentExtensionContainerList>
{
	enum { WithNetDeltaSerializer = true };
};