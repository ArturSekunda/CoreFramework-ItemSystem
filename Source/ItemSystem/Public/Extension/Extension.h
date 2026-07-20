// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Extension.generated.h"

class UPDA_Extension;
/**
 * 
 */
UCLASS(Abstract,Blueprintable)
class ITEMSYSTEM_API UExtension : public UObject
{
	GENERATED_BODY()
	
	bool bIsInitialized = false;
	
	protected:
	
	virtual void OnExtensionInitialized() {}
	
	protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Extension | PDA")
	TObjectPtr<UPDA_Extension> Extension_PDA;
	
public:
	
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
	
	UPDA_Extension* GetExtensionPDA() const { return Extension_PDA; }
	
	
};
