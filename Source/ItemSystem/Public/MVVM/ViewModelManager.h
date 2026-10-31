// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Pawn.h"

#include "Inventory/InventoryComponent.h"
#include "Equipment/EquipmentComponent.h"

#include "ViewModelManager.generated.h"


enum class ESaveLoadResult : uint8;
class UExtension;
class UInventoryExtension;
class UVM_EquipmentExtension;
class UVM_InventoryExtension;
class UItemInstance;
class UVM_ItemBase;

// UVM_ItemBase
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVMItemCreatedDelegate, UVM_ItemBase*, VM_ItemBase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVMItemRemovedDelegate, UVM_ItemBase*, VM_ItemBase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllVMItemDataDeletedDelegate);

// UVM_InventoryExtension
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVMInventoryExtensionCreatedDelegate, UVM_InventoryExtension*, VM_InventoryExtension);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVMInventoryExtensionRemovedDelegate, UVM_InventoryExtension*, VM_InventoryExtension);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllVMInventoryExtensionDataDeletedDelegate);

// UVM_EquipmentExtension
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVMEquipmentExtensionCreatedDelegate, UVM_EquipmentExtension*, VM_EquipmentExtension);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVMEquipmentExtensionRemovedDelegate, UVM_EquipmentExtension*, VM_EquipmentExtension);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllVMEquipmentExtensionDataDeletedDelegate);


UENUM(BlueprintType)
enum class EExtensionViewModelType : uint8
{
	InventoryExtension UMETA(DisplayName="InventoryExtension"),
	EquipmentExtension UMETA(DisplayName="EquipmentExtension")
};

/*
 * HELLO! This class is responsible for managing all the ViewModels in the game.
 * It provides methods for creating, removing, and clearing ViewModels.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ITEMSYSTEM_API UViewModelManager : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(Transient)
	TWeakObjectPtr<APawn> VMMOwner;
	
	void HandleNewPawn(APawn* NewPawn);
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UInventoryComponent> BoundInventory;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UEquipmentComponent> BoundEquipment;
	
protected:
	
	UPROPERTY(VisibleInstanceOnly, Category = "MVVM | Items")
	TArray<TObjectPtr<UVM_ItemBase>> AllVMItems;
	
	UPROPERTY(VisibleInstanceOnly, Category = "MVVM | InventoryExtensions")
	TArray<TObjectPtr<UVM_InventoryExtension>> AllVMInventoryExtensions;
	
	UPROPERTY(VisibleInstanceOnly, Category = "MVVM | EquipmentExtensions")
	TArray<TObjectPtr<UVM_EquipmentExtension>> AllVMEquipmentExtensions;

protected: // Handles 
	
	/**
	 * Handles the result of a loaded inventory. Virtual if you want to override it in a subclass.
	 * @param Result - The result of the inventory loading operation.
	 */
	UFUNCTION()
	virtual void Handle_LoadedInventory(ESaveLoadResult Result);
	
	/**
	 * Handles the result of a loaded equipment. Virtual if you want to override it in a subclass.
	 * @param Result - The result of the equipment loading operation.
	 */
	UFUNCTION()
	virtual void Handle_LoadedEquipment(ESaveLoadResult Result);
	
public:
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void BeginPlay() override;

	UViewModelManager();

public: // For UVM_ItemBase
	/*
	 * Will be called when a new VM_ItemBase is created.
	 */
	UPROPERTY(BlueprintAssignable, Category = "MVVM | Items")
	FOnVMItemCreatedDelegate OnVMItemCreated;
	
	/*
	 * Will be called when a VM_ItemBase is removed.
	 */
	UPROPERTY(BlueprintAssignable, Category = "MVVM | Items")
	FOnVMItemRemovedDelegate OnVMItemRemoved;
	
	/*
	 * Will be called when all VM_ItemBase are removed.
	 */
	UPROPERTY(BlueprintAssignable, Category = "MVVM | Items")
	FOnAllVMItemDataDeletedDelegate OnAllVMItemDataDeleted;
	
	UFUNCTION(BlueprintCallable, Category = "MVVM | Items")
	UVM_ItemBase* GetVMItemByGuid(FGuid ItemGuid) const;
	
	UFUNCTION(BlueprintCallable, Category = "MVVM | Items")
	void CreateVMItem(UItemInstance* ItemInstance);
	
	UFUNCTION(BlueprintCallable, Category = "MVVM | Items")
	void RemoveVMItem(FGuid ItemGuid);
	
	UFUNCTION(BlueprintCallable, Category = "MVVM | Items")
	void ClearVMItemData();
	
public: // For UVM_InventoryExtension & UVM_EquipmentExtension
	
	/*
	 * Will be called when a new VM_InventoryExtension is created.
	 */
	UPROPERTY(BlueprintAssignable, Category = "MVVM | InventoryExtensions")
	FOnVMInventoryExtensionCreatedDelegate OnVMInventoryExtensionCreated;
	
	/*
	 * Will be called when a VM_InventoryExtension is removed.
	 */
	UPROPERTY(BlueprintAssignable, Category = "MVVM | InventoryExtensions")
	FOnVMInventoryExtensionRemovedDelegate OnVMInventoryExtensionRemoved;
	
	/*
	 * Will be called when all VM_InventoryExtensions are removed.
	 */
	UPROPERTY(BlueprintAssignable, Category = "MVVM | InvEqExtensions")
	FOnAllVMInventoryExtensionDataDeletedDelegate OnAllVMInventoryExtensionDataDeleted;
	
	//================================================================================
	
	/*
	 * Will be called when a new VM_EquipmentExtension is created.
	 */
	UPROPERTY(BlueprintAssignable, Category = "MVVM | EquipmentExtensions")
	FOnVMEquipmentExtensionCreatedDelegate OnVMEquipmentExtensionCreated;
	
	/*
	 * Will be called when a VM_EquipmentExtension is removed.
	 */
	UPROPERTY(BlueprintAssignable, Category = "MVVM | EquipmentExtensions")
	FOnVMEquipmentExtensionRemovedDelegate OnVMEquipmentExtensionRemoved;
	
	/*
	 * Will be called when all VM_EquipmentExtensions are removed.
	 */
	UPROPERTY(BlueprintAssignable, Category = "MVVM | EquipmentExtensions")
	FOnAllVMEquipmentExtensionDataDeletedDelegate OnAllVMEquipmentExtensionDataDeleted;
	
	//================================================================================
	
	UFUNCTION(BlueprintCallable, Category = "MVVM | InvEqExtensions")
	void CreateVMExtension(UExtension* NewExtension);
	
	UFUNCTION(BlueprintCallable, Category = "MVVM | InvEqExtensions")
	void RemoveVMExtension(UExtension* ExtensionToRemove);
	
	UFUNCTION(BlueprintCallable, Category = "MVVM | InvEqExtensions")
	void ClearVMExtensions(EExtensionViewModelType ExtensionType);
	
	/*
	 * I don't think that you need getter for UVM_InventoryExtension or UVM_EquipmentExtension...
	 * If you need it just add it here.
	 */
	
	
	
	
};
