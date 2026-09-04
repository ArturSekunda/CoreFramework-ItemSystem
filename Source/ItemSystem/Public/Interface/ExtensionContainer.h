// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ExtensionContainer.generated.h"

class UExtension;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UExtensionContainer : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ITEMSYSTEM_API IExtensionContainer
{
	GENERATED_BODY()
	
protected:
	
	/*
	 * Prepare default extensions for the Inventory/Item Instance/Equipment.
	 * REMEMBER: Set default extensions bShouldBeSaved to false.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Extensions")
	void PrepareDefaultExtensions();

	
public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Extensions", meta=(DisplayName="Get Extensions"))
	TArray<UExtension*> GetExtensions_BP() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Extensions", meta=(DeterminesOutputType="ExtensionClass"))
	UExtension* FindExtension(TSubclassOf<UExtension> ExtensionClass) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Extensions")
	bool AddExtensionInstance(UExtension* NewExtension);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Extensions", meta=(DeterminesOutputType="ExtensionClass"))
	UExtension* AddNewExtensionByClass(TSubclassOf<UExtension> ExtensionClass);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Extensions")
	bool RemoveExtension(UExtension* ExtensionToRemove);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Extensions")
	void RemoveAllExtensions();
};
