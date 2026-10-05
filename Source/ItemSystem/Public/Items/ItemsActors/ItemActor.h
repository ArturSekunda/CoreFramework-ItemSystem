// CoreFramework Item System. Copyright Artur "Darkowy" Sekunda. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

class UItemInstance;

UCLASS(Abstract, Blueprintable)
class ITEMSYSTEM_API AItemActor : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(ReplicatedUsing=OnRep_ItemInstance, VisibleAnywhere, BlueprintReadOnly , Category = "Item Actor")
	TObjectPtr<UItemInstance> ItemInstance;
	
	/*
	 * Called when the ItemInstance is replicated to clients. 
	 * Override this function in derived classes to handle any additional logic when the ItemInstance is updated.
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Actor")
	virtual void OnRep_ItemInstance() { }
	
	UFUNCTION(BlueprintCallable, Category = "Item Actor")
	virtual void DeinitializeItemActor();
	
public:
	
	AItemActor();

	virtual void Tick(float DeltaTime) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	
	UFUNCTION(BlueprintPure, Category = "Item Actor")
	UItemInstance* GetItemInstance() const;
	
	UFUNCTION(BlueprintCallable, Category = "Item Actor")
	virtual void InitializeItemActor(UItemInstance* InItemInstance);
	
};
