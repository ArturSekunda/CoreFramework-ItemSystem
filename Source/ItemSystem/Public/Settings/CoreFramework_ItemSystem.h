// 

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CoreFramework_ItemSystem.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Item System"))
class ITEMSYSTEM_API UCoreFramework_ItemSystem : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	/**
	 * You need to set this value in the project settings to make sure that the item system works correctly.
	 * This value is used to determine the default item type for primary assets.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FName DefaultPrimaryAssetIdItemType;
};
