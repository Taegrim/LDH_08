#pragma once

#include "CoreMinimal.h"
#include "SpawnItemRow.generated.h"

USTRUCT(BlueprintType)
struct FSpawnItemRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<AActor> ItemClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float SpawnChance;
};

