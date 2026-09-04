// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "PDA_Inventory.generated.h"

class UInventoryExtension;
/**
 * This's a base class for Inventory Primary Data Asset.
 * You can create your own Inventory PDA by inheriting from this class and adding your own properties and functions.
 */
UCLASS()
class ITEMSYSTEM_API UPDA_Inventory : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public: 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Inventory Data | Default Extensions")
	TArray<TObjectPtr<UInventoryExtension>> DefaultExtensions;
};
