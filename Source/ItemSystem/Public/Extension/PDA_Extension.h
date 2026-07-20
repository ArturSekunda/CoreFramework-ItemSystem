// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataAsset.h"
#include "PDA_Extension.generated.h"

class UVM_ItemExtension;
/**
 * 
 */
UCLASS()
class ITEMSYSTEM_API UPDA_Extension : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Extension | MVVM")
	TSoftClassPtr<UVM_ItemExtension> ExtensionVMClass;
	
};
