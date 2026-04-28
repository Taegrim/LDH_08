#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BombardmentVolume.generated.h"

class UBoxComponent;

UCLASS()
class LDH_08_API ABombardmentVolume : public AActor
{
	GENERATED_BODY()

public:
	ABombardmentVolume();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Bombardment")
	void StartBombardment();

	UFUNCTION(BlueprintCallable, Category = "Bombardment")
	void StopBombardment();

protected:
    // 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bombardment|Component")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bombardment|Component")
	UBoxComponent* SpawnBox;

    // 생성할 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bombardment|Class")
	TSubclassOf<AActor> BombardmentClass;

    // 폭격 간격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bombardment|Spawn")
	float SpawnInterval;

    // 한번에 폭격할 횟수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bombardment|Spawn")
    int32 BombardmentCount;

    // 땅에 붙어서 생성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bombardment|Spawn")
	bool bTraceToGround;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedBombardments;

	FTimerHandle BombardmentTimerHandle;

	void SpawnBombardment();
	FVector GetRandomLocationInVolume() const;
	FVector GetSpawnLocation() const;
};
