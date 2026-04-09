#include "SpartaGameState.h"

#include "CoinItem.h"
#include "SpartaGameInstance.h"
#include "SpawnVolume.h"
#include "Kismet/GameplayStatics.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 30.f;
	CurrentLevelIndex = 0;
	MaxLevel = 3;
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();
	
	StartLevel();
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
		{
			SpartaGameInstance->AddScore(Amount);
		}
	}
}

void ASpartaGameState::OnGameOver()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Over!"));
}

void ASpartaGameState::StartLevel()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
		{
			CurrentLevelIndex = SpartaGameInstance->GetCurrentLevel();
		}
	}
	
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	
	// 배치된 SpawnVolume 전부 찾기
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
	
	for (int32 i = 0; i < MaxSpawnItems; ++i)
	{
		if (FoundVolumes.Num() > 0)
		{
			// 하나만 깔 예정이므로 0번 인덱스의 스폰볼륨 가져옴
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					++SpawnedCoinCount;
				}
			}
		}
	}
	
	// 30초 제한시간
	GetWorldTimerManager().SetTimer(
		LevelTimerHandle,
		this,
		&ASpartaGameState::OnLevelTimeUp,
		LevelDuration,
		false
	);
	
	UE_LOG(LogTemp, Warning, TEXT("Level %d Start, Spawned Coin %d !"),
		CurrentLevelIndex + 1, SpawnedCoinCount);
}

void ASpartaGameState::OnLevelTimeUp()
{
	EndLevel();
}

void ASpartaGameState::OnCoinCollected()
{
	++CollectedCoinCount;
	
	UE_LOG(LogTemp, Warning, TEXT("Coin: %d / %d"),
		CollectedCoinCount, SpawnedCoinCount);
	
	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		EndLevel();
	}
}

void ASpartaGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	++CurrentLevelIndex;
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
		{
			AddScore(Score);
			SpartaGameInstance->SetCurrentLevel(CurrentLevelIndex);
		}
	}
	
	if (CurrentLevelIndex >= MaxLevel)
	{
		OnGameOver();
		return;
	}
	
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}
