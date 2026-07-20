// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InventoryData/InvData.h"

#include "InventoryComponent.generated.h"

class UPDA_Inventory;
class UInventoryExtensionContainer;
enum class EInventorySaveLoadResult : uint8;

/*
 * This's base class for Inventory.
 * I leaved this class empty because maybe you would like to make a singleplayer game.
 */
UCLASS(Abstract,ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ITEMSYSTEM_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary Data Asset")
	TObjectPtr<UPDA_Inventory> Inventory_PDA;
	
	virtual void BeginPlay() override;
	
	UPROPERTY(Replicated, VisibleAnywhere,  BlueprintReadOnly, Category = "Inventory")
	FInventoryContainerList InventoryContainer;
	
	UPROPERTY(Replicated, VisibleAnywhere,  BlueprintReadOnly, Category = "Inventory | Extensions")
	TObjectPtr<UInventoryExtensionContainer> InventoryExtensionContainer;

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UInventoryComponent();
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UPDA_Inventory* GetInventoryPDA() const { return Inventory_PDA; }
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UInventoryExtensionContainer* GetInventoryExtensionContainer() const { return InventoryExtensionContainer; }
};
