// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Interface/ContainersSerializers.h"
#include "Interface/ExtensionContainer.h"

#include "EquipmentComponent.generated.h"


class UPDA_Equipment;
class UEquipmentExtensionContainer;


UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ITEMSYSTEM_API UEquipmentComponent : public UActorComponent, public IExtensionContainer
{
	GENERATED_BODY()


protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Primary Data Asset")
	TObjectPtr<UPDA_Equipment> Equipment_PDA;
	
	UPROPERTY(Replicated, VisibleAnywhere,  BlueprintReadOnly, Category = "Equipment | Extensions")
	FEquipmentExtensionContainerList EquipmentExtensionSerializer;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UEquipmentComponent();
	
	UFUNCTION(BlueprintPure, Category = "Equipment")
	UPDA_Equipment* GetEquipmentPDA() const { return Equipment_PDA; }
	
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory | Save")
	void GetSaveData(FEquipmentSaveData& OutSaveData) const;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory | Save")
	void LoadFromSaveData(const FEquipmentSaveData& InSaveData);
	
	
public: // IExtensionContainer interface
	
	virtual void PrepareDefaultExtensions_Implementation() override;

	virtual bool AddExtensionInstance_Implementation(UExtension* NewExtension) override;
	
	virtual UExtension* AddNewExtensionByClass_Implementation(TSubclassOf<UExtension> ExtensionClass) override;
	
	virtual UExtension* FindExtension_Implementation(TSubclassOf<UExtension> ExtensionClass) const override;
	
	virtual TArray<UExtension*> GetExtensions_BP_Implementation() const override;
	
	virtual void RemoveAllExtensions_Implementation() override;
	
	virtual bool RemoveExtension_Implementation(UExtension* ExtensionToRemove) override;
};
