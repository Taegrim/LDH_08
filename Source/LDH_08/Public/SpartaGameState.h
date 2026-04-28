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

    // 레벨 관련
	void StartLevel();
	void OnLevelTimeUp();
	void EndLevel();

    // HUD 갱신
	void UpdateHUD();

    // 웨이브 관련
	void StartWave();
	void OnCoinCollected();
	void OnWaveTimeUp();
	void EndWave();

    // 웨이브 환경 이벤트 관련
	void StartWaveEnvironmentEvent(const FWaveData& WaveData);
	void StopWaveEnvironmentEvent();
	void ShowWaveEnvironmentMessage(EWaveEnvironmentEvent EnvironmentEvent);

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

	// 생성한 아이템들을 저장하는 배열, 남은 아이템 비울때 사용
	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedItems;

	// 타이머 핸들러
	FTimerHandle WaveTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;
};
