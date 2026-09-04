// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Interface/ContainersSerializers.h"
#include "Interface/ExtensionContainer.h"

#include "InventoryData/InvData.h"

#include "Subsystems/SaveData/ObjectsSaveData.h"

#include "InventoryComponent.generated.h"


class UPDA_Inventory;
enum class ESaveLoadResult : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryLoading, ESaveLoadResult, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventorySaving, ESaveLoadResult, Result);

/*
 * This's base class for Inventory.
 * It provides the basic functionality for managing an inventory.
 */
UCLASS(Abstract,ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ITEMSYSTEM_API UInventoryComponent : public UActorComponent, public IExtensionContainer
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary Data Asset")
	TObjectPtr<UPDA_Inventory> Inventory_PDA;
	
	virtual void BeginPlay() override;
	
	UPROPERTY(Replicated, VisibleInstanceOnly,  BlueprintReadOnly, Category = "Inventory")
	FInventoryContainerList InventoryContainer;
	
	UPROPERTY(Replicated, VisibleAnywhere,  BlueprintReadOnly, Category = "Inventory | Extensions")
	FInventoryExtensionContainerList InventoryExtensionSerializer;

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UInventoryComponent();
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UPDA_Inventory* GetInventoryPDA() const { return Inventory_PDA; }
	
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory | Save")
	ESaveLoadResult GetSaveData(FInventorySaveData& OutSaveData) const;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory | Save")
	ESaveLoadResult LoadFromSaveData(const FInventorySaveData& InSaveData);
	
	/*
	 *  There's only logic for AddReplicatedSubObject().
	 *  Use this before adding the item to the inventory, because it will be replicated to the clients.
	 */
	virtual void ReplicateItemAndExtensions(TObjectPtr<UItemInstance>& InItemInstance);
	
	/*
	 *  There's only logic for RemoveReplicatedSubObject().
	 *  Use this before removing the item from the inventory, because it will be replicated to the clients.
	 */
	virtual void RemoveReplicatedItemAndExtensions(TObjectPtr<UItemInstance>& InItemInstance);
	
public: // Delegates
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory | Delegates")
	FOnInventoryLoading OnInventoryLoading;
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory | Delegates")
	FOnInventorySaving OnInventorySaving;

public: // IExtensionContainer interface

	virtual void PrepareDefaultExtensions_Implementation() override;

	virtual bool AddExtensionInstance_Implementation(UExtension* NewExtension) override;
	
	virtual UExtension* AddNewExtensionByClass_Implementation(TSubclassOf<UExtension> ExtensionClass) override;
	
	virtual UExtension* FindExtension_Implementation(TSubclassOf<UExtension> ExtensionClass) const override;
	
	virtual TArray<UExtension*> GetExtensions_BP_Implementation() const override;
	
	virtual void RemoveAllExtensions_Implementation() override;
	
	virtual bool RemoveExtension_Implementation(UExtension* ExtensionToRemove) override;
};
