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
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly , Category = "Item Actor")
	TObjectPtr<UItemInstance> ItemInstance;
	
public:
	
	AItemActor();

	virtual void Tick(float DeltaTime) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
public:
	
	UFUNCTION(BlueprintPure, Category = "Item Actor")
	UItemInstance* GetItemInstance() const;
	
	UFUNCTION(BlueprintCallable, Category = "Item Actor")
	void InitializeItem(UItemInstance* InItemInstance);
	
};
