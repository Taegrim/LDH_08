#include "SpartaGameInstance.h"

USpartaGameInstance::USpartaGameInstance() : TotalScore(0), CurrentLevelIndex(0)
{
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

void USpartaGameInstance::SetCurrentLevel(int32 LevelIndex)
{
	CurrentLevelIndex = LevelIndex;
}
