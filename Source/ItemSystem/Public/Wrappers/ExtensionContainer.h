// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WrapperData.h"

#include "UObject/Object.h"
#include "ExtensionContainer.generated.h"

class UItemExtension;
class UInventoryExtension;
class UEquipmentExtension;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemExtensionsChangedDelegate);

UCLASS(BlueprintType, Blueprintable)
class ITEMSYSTEM_API UItemExtensionContainer : public UObject
{
	GENERATED_BODY()
	
public:
	UItemExtensionContainer();
	
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintAssignable, Category = "Item | Extensions | Delegates")
	FOnItemExtensionsChangedDelegate OnItemExtensionsChanged;

protected:
	UPROPERTY(Replicated)
	FItemExtensionContainerList ExtensionList;
	
public:

	const FItemExtensionContainerList& GetExtensionList() const { return ExtensionList; }
	
	UFUNCTION(BlueprintPure, Category = "Extensions", meta=(DisplayName="Get Extensions"))
	TArray<UItemExtension*> GetExtensions_BP() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "Extensions", meta=(DeterminesOutputType="ExtensionClass"))
	UItemExtension* FindExtension(TSubclassOf<UItemExtension> ExtensionClass) const;

	UFUNCTION(BlueprintCallable, Category = "Extensions")
	bool AddExtensionInstance(UItemExtension* NewExtension);

	UFUNCTION(BlueprintCallable, Category = "Extensions", meta=(DeterminesOutputType="ExtensionClass"))
	UItemExtension* AddNewExtensionByClass(TSubclassOf<UItemExtension> ExtensionClass);
	
	UFUNCTION(BlueprintCallable, Category = "Extensions")
	bool RemoveExtension(UItemExtension* ExtensionToRemove);

	UFUNCTION(BlueprintCallable, Category = "Extensions")
	void RemoveAllExtensions();
};

UCLASS(BlueprintType, Blueprintable)
class ITEMSYSTEM_API UInventoryExtensionContainer : public UObject
{
	GENERATED_BODY()
	
public:
	UInventoryExtensionContainer();
	
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Replicated)
	FInventoryExtensionContainerList ExtensionList;
	
public:

	const FInventoryExtensionContainerList& GetExtensionList() const { return ExtensionList; }
	
	UFUNCTION(BlueprintPure, Category = "Extensions", meta=(DisplayName="Get Extensions"))
	TArray<UInventoryExtension*> GetExtensions_BP() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "Extensions", meta=(DeterminesOutputType="ExtensionClass"))
	UInventoryExtension* FindExtension(TSubclassOf<UInventoryExtension> ExtensionClass) const;

	UFUNCTION(BlueprintCallable, Category = "Extensions")
	bool AddExtensionInstance(UInventoryExtension* NewExtension);

	UFUNCTION(BlueprintCallable, Category = "Extensions", meta=(DeterminesOutputType="ExtensionClass"))
	UInventoryExtension* AddNewExtensionByClass(TSubclassOf<UInventoryExtension> ExtensionClass);
	
	UFUNCTION(BlueprintCallable, Category = "Extensions")
	bool RemoveExtension(UInventoryExtension* ExtensionToRemove);

	UFUNCTION(BlueprintCallable, Category = "Extensions")
	void RemoveAllExtensions();
};

UCLASS(BlueprintType, Blueprintable)
class ITEMSYSTEM_API UEquipmentExtensionContainer : public UObject
{
	GENERATED_BODY()
	
public:
	UEquipmentExtensionContainer();
	
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Replicated)
	FEquipmentExtensionContainerList ExtensionList;
	
public:

	const FEquipmentExtensionContainerList& GetExtensionList() const { return ExtensionList; }
	
	UFUNCTION(BlueprintPure, Category = "Extensions", meta=(DisplayName="Get Extensions"))
	TArray<UEquipmentExtension*> GetExtensions_BP() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category = "Extensions", meta=(DeterminesOutputType="ExtensionClass"))
	UEquipmentExtension* FindExtension(TSubclassOf<UEquipmentExtension> ExtensionClass) const;

	UFUNCTION(BlueprintCallable, Category = "Extensions")
	bool AddExtensionInstance(UEquipmentExtension* NewExtension);

	UFUNCTION(BlueprintCallable, Category = "Extensions", meta=(DeterminesOutputType="ExtensionClass"))
	UEquipmentExtension* AddNewExtensionByClass(TSubclassOf<UEquipmentExtension> ExtensionClass);
	
	UFUNCTION(BlueprintCallable, Category = "Extensions")
	bool RemoveExtension(UEquipmentExtension* ExtensionToRemove);

	UFUNCTION(BlueprintCallable, Category = "Extensions")
	void RemoveAllExtensions();
};
