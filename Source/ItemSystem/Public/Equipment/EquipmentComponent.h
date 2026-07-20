// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"


class UPDA_Equipment;
class UEquipmentExtensionContainer;

UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ITEMSYSTEM_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()


protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary Data Asset")
	TObjectPtr<UPDA_Equipment> Equipment_PDA;
	
	UPROPERTY(Replicated, VisibleAnywhere,  BlueprintReadOnly, Category = "Equipment | Extensions")
	TObjectPtr<UEquipmentExtensionContainer> EquipmentExtensionContainer;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
	UEquipmentComponent();
	
	UFUNCTION(BlueprintPure, Category = "Equipment")
	UPDA_Equipment* GetEquipmentPDA() const { return Equipment_PDA; }
	
	UFUNCTION(BlueprintPure, Category = "Equipment")
	UEquipmentExtensionContainer* GetEquipmentExtensionContainer() const { return EquipmentExtensionContainer; }
};
