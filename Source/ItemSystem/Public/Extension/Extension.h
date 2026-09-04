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
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Extension | MVVM")
	TObjectPtr<UVM_Extension> VM_Instance;
	
public:
	
	void InitializeExtension()
	{
		if (bIsInitialized) return;
		
		if (VM_ExtensionClass != nullptr)
		{
			VM_Instance = NewObject<UVM_Extension>(this, VM_ExtensionClass);
		}

		bIsInitialized = true;
		
		OnExtensionInitialized();
	}
	
	virtual UWorld* GetWorld() const override
	{
		if (HasAllFlags(RF_ClassDefaultObject)) return nullptr;
		return GetOuter() ? GetOuter()->GetWorld() : nullptr;
	}
	
	TObjectPtr<UVM_Extension> GetVM() const { return VM_Instance; }
	
	UFUNCTION(BlueprintNativeEvent, Category= "Extension | Save")
	void GetSaveData(FSaveExtensionData& OutData);
	
	UFUNCTION(BlueprintNativeEvent, Category= "Extension | Save")
	void LoadFromSaveData(const FSaveExtensionData& InData);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Extension | Save")
	bool bShouldBeSaved = false;
	
	
	
};
