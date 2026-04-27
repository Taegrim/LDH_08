#include "SpartaGameInstance.h"

USpartaGameInstance::USpartaGameInstance() : TotalScore(0), CurrentLevelIndex(0)
{
}

void USpartaGameInstance::Init()
{
	Super::Init();

	CurrentLevelIndex = 0;
	TotalScore = 0;
}

void USpartaGameInstance::AddScore(int32 Amount)
{
	TotalScore += Amount;
	UE_LOG(LogTemp, Warning, TEXT("TotalScore = %d"), TotalScore);
}

int32 USpartaGameInstance::GetCurrentLevel() const
{
	return CurrentLevelIndex;
}

int32 USpartaGameInstance::GetTotalScore() const
{
	return TotalScore;
}

void USpartaGameInstance::SetCurrentLevel(int32 LevelIndex)
{
	CurrentLevelIndex = LevelIndex;
}
