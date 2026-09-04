#pragma once

#include "CoreMinimal.h"

#include "Items/ItemsData/EditableItemData.h"

#include "objectsSaveData.generated.h"

struct FSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FSaveGameArchive(FArchive& InInnerArchive)
		: FObjectAndNameAsStringProxyArchive(InInnerArchive, true)
	{
		ArIsSaveGame = true;
	}
};

UENUM(BlueprintType)
enum class ESaveLoadResult : uint8
{
	Success UMETA(DisplayName = "Success"),
	Corrupted UMETA(DisplayName = "Corrupted"),
	Failed UMETA(DisplayName = "Failed"),
};

USTRUCT(BlueprintType)
struct FSaveExtensionData
{
	GENERATED_BODY()
	
	UPROPERTY()
	FSoftClassPath ExtensionClass;

	UPROPERTY()
	TArray<uint8> ByteData;

};

USTRUCT(BlueprintType)
struct FItemInstanceSaveData
{
	GENERATED_BODY()
	
	UPROPERTY()
	FGuid Guid;
	
	UPROPERTY()
	FEditableItemBaseData EditableBaseData;
	
	UPROPERTY()
	FString ItemPDAPath;
	
	UPROPERTY()
	TArray<FSaveExtensionData> ExtensionsData;
	
};

USTRUCT(BlueprintType)
struct FInventorySlotSaveData
{
	GENERATED_BODY()
	
	UPROPERTY()
	FItemInstanceSaveData ItemInstanceData;
	
	UPROPERTY()
	FGuid SlotGuid;
	
	UPROPERTY()
	int32 SlotIndex;
	
};

USTRUCT(BlueprintType)
struct FInventorySaveData
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString InventoryPDAPath;
	
	UPROPERTY()
	TArray<FInventorySlotSaveData> SlotsSaveData;
	
	UPROPERTY()
	TArray<FSaveExtensionData> ExtensionsData;
	
};

USTRUCT(BlueprintType)
struct FEquipmentSaveData
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString EquipmentPDAPath;
	
	UPROPERTY()
	TArray<FSaveExtensionData> ExtensionsData;
};


