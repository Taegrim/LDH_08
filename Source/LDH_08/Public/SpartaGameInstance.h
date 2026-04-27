#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SpartaGameInstance.generated.h"

UCLASS()
class LDH_08_API USpartaGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	USpartaGameInstance();

	UFUNCTION(BlueprintCallable, Category = "GameData")
	virtual void Init() override;
	UFUNCTION(BlueprintCallable, Category = "GameData")
	void AddScore(int32 Amount);
	UFUNCTION(BlueprintPure, Category = "GameData")
	int32 GetCurrentLevel() const;
	UFUNCTION(BlueprintPure, Category = "GameData")
	int32 GetTotalScore() const;

	UFUNCTION(BlueprintCallable, Category = "GameData")
	void SetCurrentLevel(int32 LevelIndex);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 TotalScore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 CurrentLevelIndex;
};
