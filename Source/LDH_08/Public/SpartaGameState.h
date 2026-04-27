#pragma once

#include "CoreMinimal.h"
#include "WaveData.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

UCLASS()
class LDH_08_API ASpartaGameState : public AGameState
{
	GENERATED_BODY()

public:
	ASpartaGameState();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnGameOver();

	void StartLevel();
	void OnLevelTimeUp();
	void OnCoinCollected();
	void EndLevel();
	void UpdateHUD();

	void StartWave();
	void OnWaveTimeUp();
	void EndWave();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TArray<FWaveData> Waves;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 CurrentWaveIndex;

	// 소환한 아이템들을 저장하는 배열
	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedItems;

	// 타이머 핸들러
	FTimerHandle WaveTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;
};
