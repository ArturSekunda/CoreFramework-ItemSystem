// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "PDA_Equipment.generated.h"

class UEquipmentExtension;
/**
 * Base class for equipment primary data assets. 
 * You can create your own primary data asset class by inheriting from this class and adding your own properties and functions.
 * Remember! This class is responsible for NON-Editable data in runtime. Don't make mess!
 */
UCLASS()
class ITEMSYSTEM_API UPDA_Equipment : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Equipment Data | Default Extensions")
	TArray<TObjectPtr<UEquipmentExtension>> DefaultExtensions;
};
