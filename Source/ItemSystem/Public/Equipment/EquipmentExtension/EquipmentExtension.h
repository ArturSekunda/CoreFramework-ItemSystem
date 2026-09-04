// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Extension/Extension.h"
#include "EquipmentExtension.generated.h"

class UEquipmentComponent;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class ITEMSYSTEM_API UEquipmentExtension : public UExtension
{
	GENERATED_BODY()
protected:
	UPROPERTY(Transient)
	TWeakObjectPtr<UEquipmentComponent> OwningEquipmentComponent;
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Equipment Extension")
	void SetOwningEquipmentComponent(UEquipmentComponent* InOwner);
	
	UFUNCTION(BlueprintPure, Category = "Equipment Extension")
	UEquipmentComponent* GetOwningEquipmentComponent() const;
};
