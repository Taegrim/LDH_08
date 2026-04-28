#include "SpartaGameState.h"

#include "CoinItem.h"
#include "BombardmentVolume.h"
#include "SpartaCharacter.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "SpikeTrapVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ASpartaGameState::ASpartaGameState()
{
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	CurrentLevelIndex = 0;
	MaxLevel = 3;
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	// UpdateHUD();
	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true
		);
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

void ASpartaGameState::StartLevel()
{
	// 레벨 시작 시 HUD 새로 생성 및 초기화
	APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PlayerController) return;

	if (ASpartaPlayerController* SpartaController = Cast<ASpartaPlayerController>(PlayerController))
	{
		SpartaController->ShowGameHUD();
	}

    // 걸린 상태이상 초기화
    if (ASpartaCharacter* SpartaCharacter = Cast<ASpartaCharacter>(PlayerController->GetPawn()))
    {
        SpartaCharacter->ResetStatusEffects();
    }

	if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GetGameInstance()))
	{
		CurrentLevelIndex = SpartaGameInstance->GetCurrentLevel();
	}

	CurrentWaveIndex = 0;
	StartWave();
}

void ASpartaGameState::OnLevelTimeUp()
{
	EndLevel();
}

void ASpartaGameState::OnCoinCollected()
{
	++CollectedCoinCount;

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		EndWave();
	}
}

void ASpartaGameState::EndLevel()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
		{
			++CurrentLevelIndex;
			SpartaGameInstance->SetCurrentLevel(CurrentLevelIndex);

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
	}
}

void ASpartaGameState::OnGameOver()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);
	StopWaveEnvironmentEvent();

	APlayerController* PlayerController = (GetWorld()) ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PlayerController) return;

	if (ASpartaPlayerController* SpartaController = Cast<ASpartaPlayerController>(PlayerController))
	{
		SpartaController->SetPause(true);
		SpartaController->ShowMainMenu(true);
	}
}

void ASpartaGameState::UpdateHUD()
{
	APlayerController* PlayerController = (GetWorld()) ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PlayerController) return;

	ASpartaPlayerController* SpartaController = Cast<ASpartaPlayerController>(PlayerController);
	if (!SpartaController) return;

	UUserWidget* HUDWidget = SpartaController->GetHUDWidget();
	if (!HUDWidget) return;

	if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TimeText"))))
	{
		float RemainTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
		TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time : %.1f"), RemainTime)));
	}

	if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("ScoreText"))))
	{
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
			{
				ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score : %d"),
					SpartaGameInstance->GetTotalScore())));
			}
		}
	}

	if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("LevelText"))))
	{
		LevelIndexText->SetText(FText::FromString(
			FString::Printf(TEXT("Level : %d, Wave : %d"), CurrentLevelIndex + 1, CurrentWaveIndex + 1)
			));
	}
}

void ASpartaGameState::StartWave()
{
	// 유효한 웨이브가 아니라면 현재 레벨 종료
	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		EndLevel();
		return;
	}

	const FWaveData& CurrentWave = Waves[CurrentWaveIndex];

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	// 배치된 SpawnVolume 전부 찾기
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	if (FoundVolumes.Num() > 0)
	{
		// 하나만 깔 예정이므로 0번 인덱스의 스폰볼륨 가져옴
		if (ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]))
		{
			for (int32 i = 0; i < CurrentWave.SpawnItemCount; ++i)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();

				if (SpawnedActor)
				{
					SpawnedItems.Add(SpawnedActor);

					if (SpawnedActor->IsA(ACoinItem::StaticClass()))
					{
						++SpawnedCoinCount;
					}
				}
			}
		}
	}

    // 환경 이벤트 시작
	StartWaveEnvironmentEvent(CurrentWave);

    // 제한 시간 세팅
	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ASpartaGameState::OnWaveTimeUp,
		CurrentWave.WaveDuration,
		false
	);
}

void ASpartaGameState::OnWaveTimeUp()
{
	EndWave();
}

void ASpartaGameState::EndWave()
{
	// 기존 웨이브 관련 정보 제거
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	StopWaveEnvironmentEvent();

	for (const auto& Item : SpawnedItems)
	{
		if (IsValid(Item))
		{
			Item->Destroy();
		}
	}
	SpawnedItems.Empty();

    // 다음 웨이브 진입 처리
	++CurrentWaveIndex;

	if (Waves.IsValidIndex(CurrentWaveIndex))
	{
		StartWave();
	}
	else
	{
		EndLevel();
	}
}

// 환경 이벤트 시작 함수
void ASpartaGameState::StartWaveEnvironmentEvent(const FWaveData& WaveData)
{
	StopWaveEnvironmentEvent();
	ShowWaveEnvironmentMessage(WaveData.EnvironmentEvent);

	switch (WaveData.EnvironmentEvent)
	{
	case EWaveEnvironmentEvent::SpikeTrap:
	{
		TArray<AActor*> FoundVolumes;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpikeTrapVolume::StaticClass(), FoundVolumes);

		for (AActor* ActorVolume : FoundVolumes)
		{
			if (ASpikeTrapVolume* SpikeTrapVolume = Cast<ASpikeTrapVolume>(ActorVolume))
			{
				SpikeTrapVolume->ActivateSpikeTraps();
			}
		}
		break;
	}

	case EWaveEnvironmentEvent::Bombardment:
	{
		TArray<AActor*> FoundVolumes;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABombardmentVolume::StaticClass(), FoundVolumes);

		for (AActor* ActorVolume : FoundVolumes)
		{
			if (ABombardmentVolume* BombardmentVolume = Cast<ABombardmentVolume>(ActorVolume))
			{
				BombardmentVolume->StartBombardment();
			}
		}
		break;
	}

	default:
		break;
	}
}

// 환경 이벤트 중지 함수
void ASpartaGameState::StopWaveEnvironmentEvent()
{
    // 배치된 모든 스파이크 함정 볼륨 가져옴
	TArray<AActor*> FoundSpikeVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpikeTrapVolume::StaticClass(), FoundSpikeVolumes);

    // 스파이크 함정 비활성화
	for (AActor* VolumeActor : FoundSpikeVolumes)
	{
		if (ASpikeTrapVolume* SpikeTrapVolume = Cast<ASpikeTrapVolume>(VolumeActor))
		{
			SpikeTrapVolume->DeactivateSpikeTraps();
		}
	}

    // 배치된 모든 폭격 볼륨 가져옴
	TArray<AActor*> FoundBombardmentVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABombardmentVolume::StaticClass(), FoundBombardmentVolumes);

    // 폭격 작동 중지
	for (AActor* VolumeActor : FoundBombardmentVolumes)
	{
		if (ABombardmentVolume* BombardmentVolume = Cast<ABombardmentVolume>(VolumeActor))
		{
			BombardmentVolume->StopBombardment();
		}
	}
}

// 플레이어에게 환경 이벤트 발생을 알리는 함수
void ASpartaGameState::ShowWaveEnvironmentMessage(EWaveEnvironmentEvent EnvironmentEvent)
{
	if (EnvironmentEvent == EWaveEnvironmentEvent::None) return;

	APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	ASpartaPlayerController* SpartaController = Cast<ASpartaPlayerController>(PlayerController);
	if (!SpartaController) return;

	UUserWidget* HUDWidget = SpartaController->GetHUDWidget();
	if (!HUDWidget) return;

	UTextBlock* MessageText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("WaveEventText")));
	if (!MessageText) return;

    FString Message;

	switch (EnvironmentEvent)
	{
	case EWaveEnvironmentEvent::SpikeTrap:
	    Message = TEXT("스파이크 함정이 발동되었습니다!!!");
		break;

	case EWaveEnvironmentEvent::Bombardment:
		Message = TEXT("포격이 시작되었습니다!!!");
		break;

	default:
		return;
	}

    MessageText->SetText(FText::FromString(Message));
	MessageText->SetVisibility(ESlateVisibility::HitTestInvisible);

    if (UFunction* Func = HUDWidget->FindFunction(FName("PlayWaveEventMessageAnim")))
    {
        HUDWidget->ProcessEvent(Func, nullptr);
    }
}
