#pragma once

#include "CoreMinimal.h"
#include "ConstExtensionData.generated.h"

USTRUCT(BlueprintType)
struct FConstExtensionData
{
	GENERATED_BODY()
	
};

USTRUCT(BlueprintType)
struct FConstStatusEffectData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect Data")
	float Duration = 5.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect Data")
	float TickInterval = 0.0f;
};


USTRUCT(BlueprintType)
struct FConstWeightData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ConstWeight = 0.0f;
	
};

USTRUCT(BlueprintType)
struct FConstValueData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ConstValue = 0.0f;
	
};