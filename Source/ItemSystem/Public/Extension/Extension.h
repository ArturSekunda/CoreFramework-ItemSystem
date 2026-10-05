// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MVVM/ViewModels/VM_Extension.h"

#include "UObject/Object.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

#include "Subsystems/SaveData/ObjectsSaveData.h"

#include "Extension.generated.h"

class UVM_Extension;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ITEMSYSTEM_API UExtension : public UObject
{
	GENERATED_BODY()
	
	bool bIsInitialized = false;
	
	protected:
	
	virtual void OnExtensionInitialized() { }
	
	protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Extension | MVVM")
	TSubclassOf<UVM_Extension> VM_ExtensionClass;
	
public:
	
	virtual bool IsSupportedForNetworking() const override { return true; }
	
	void InitializeExtension()
	{
		if (bIsInitialized) return;

		bIsInitialized = true;
		
		OnExtensionInitialized();
	}
	
	virtual UWorld* GetWorld() const override
	{
		if (HasAllFlags(RF_ClassDefaultObject)) return nullptr;
		return GetOuter() ? GetOuter()->GetWorld() : nullptr;
	}
	
	const TSubclassOf<UVM_Extension>& GetVM() const { return VM_ExtensionClass; }
	
	UFUNCTION(BlueprintNativeEvent, Category= "Extension | Save")
	void GetSaveData(FSaveExtensionData& OutData);
	
	UFUNCTION(BlueprintNativeEvent, Category= "Extension | Save")
	void LoadFromSaveData(const FSaveExtensionData& InData);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Extension | Save")
	bool bShouldBeSaved = false;
	
	
	
};
