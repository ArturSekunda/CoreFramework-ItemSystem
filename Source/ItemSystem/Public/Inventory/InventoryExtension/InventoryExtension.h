// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Extension/Extension.h"

#include "Inventory/InventoryComponent.h"

#include "UObject/Object.h"
#include "InventoryExtension.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class ITEMSYSTEM_API UInventoryExtension : public UExtension
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(Transient)
	TWeakObjectPtr<UInventoryComponent> OwningInventory;
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Inventory Extension")
	void SetOwningInventory(UInventoryComponent* InOwner) { OwningInventory = InOwner; }
	
	UFUNCTION(BlueprintPure, Category = "Inventory Extension")
	UInventoryComponent* GetOwningInventory() const
	{
		if (OwningInventory.IsValid())
		{
			return OwningInventory.Get();
		}
		
		return nullptr;
	}
};
