#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpikeTrapVolume.generated.h"

class UBoxComponent;

UCLASS()
class LDH_08_API ASpikeTrapVolume : public AActor
{
	GENERATED_BODY()

public:
	ASpikeTrapVolume();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Spike Trap")
	void ActivateSpikeTraps();

	UFUNCTION(BlueprintCallable, Category = "Spike Trap")
	void DeactivateSpikeTraps();

protected:
    // 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spike Trap|Component")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spike Trap|Component")
	UBoxComponent* SpawnBox;

    // 생성할 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Trap|Class")
	TSubclassOf<AActor> SpikeTrapClass;

    // 스파이크 함정 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Trap|Spawn")
	int32 SpikeTrapCount;

    // 땅에 붙어서 생성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Trap|Spawn")
	bool bTraceToGround;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedSpikeTraps;

	FVector GetRandomLocationInVolume() const;
	FVector GetSpawnLocation() const;
};
