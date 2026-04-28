#pragma once

#include "CoreMinimal.h"
#include "WaveData.generated.h"

// 웨이브 환경 이벤트 타입
UENUM(BlueprintType)
enum class EWaveEnvironmentEvent : uint8
{
	None UMETA(DisplayName = "None"),
	SpikeTrap UMETA(DisplayName = "Spike Trap"),
	Bombardment UMETA(DisplayName = "Bombardment")
};

USTRUCT(BlueprintType)
struct FWaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 SpawnItemCount = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float WaveDuration = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Environment")
	EWaveEnvironmentEvent EnvironmentEvent = EWaveEnvironmentEvent::None;
};
