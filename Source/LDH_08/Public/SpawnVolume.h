#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnItemRow.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class LDH_08_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnVolume();
	
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	AActor* SpawnRandomItem();
	FSpawnItemRow* GetRandomItem() const;
	AActor* SpawnItem(TSubclassOf<AActor> ItemClass);
	FVector GetRandomSpawnLocation() const;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	UBoxComponent* SpawnBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	UDataTable* ItemDataTable;
};
