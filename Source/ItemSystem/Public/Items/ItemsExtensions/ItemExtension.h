// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Extension/Extension.h"

#include "UObject/Object.h"
#include "ItemExtension.generated.h"

class UItemInstance;

/*
 * Base class for item extensions. You can create your own item extension class by inheriting from this class and adding your own properties and functions.
 * Also this one is empty because you would like to make your own item extension system etc.
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class ITEMSYSTEM_API UItemExtension : public UExtension
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(Transient)
	TWeakObjectPtr<UItemInstance> OwningItem;
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Item Extension")
	void SetOwningItem(UItemInstance* InOwner);
	
	UFUNCTION(BlueprintPure, Category = "Item Extension")
	UItemInstance* GetOwningItem() const;
};
