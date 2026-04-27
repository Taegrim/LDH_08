#include "SpartaPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "SpartaGameInstance.h"
#include "SpartaGameState.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

ASpartaPlayerController::ASpartaPlayerController() :
	InputMappingContext(nullptr), MoveAction(nullptr), LookAction(nullptr), JumpAction(nullptr), SprintAction(nullptr),
	HUDWidgetClass(nullptr), HUDWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr), MainMenuWidgetInstance(nullptr)
{
}

void ASpartaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어 컨틀롤러와 연결된 플레이어 객체 가져옴
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer) return;

	UEnhancedInputLocalPlayerSubsystem * SubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!SubSystem) return;

	if (InputMappingContext)
	{
		SubSystem->AddMappingContext(InputMappingContext, 0);
	}

	// 초기 실행 시 메뉴 UI 표시
	FString MapName = (GetWorld()) ? GetWorld()->GetMapName() : "";
	if (MapName.Contains("MenuLevel"))
	{
		ShowMainMenu(false);
	}
}

UInputAction* ASpartaPlayerController::GetMoveAction() const
{
	return MoveAction;
}

UInputAction* ASpartaPlayerController::GetLookAction() const
{
	return LookAction;
}

UInputAction* ASpartaPlayerController::GetJumpAction() const
{
	return JumpAction;
}

UInputAction* ASpartaPlayerController::GetSprintAction() const
{
	return SprintAction;
}

UUserWidget* ASpartaPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void ASpartaPlayerController::ShowGameHUD()
{
	CloseWidget();

	// HUD 위젯 생성 및 표시
	if (!HUDWidgetClass) return;

	HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
	if (HUDWidgetInstance)
	{
		// 위젯 뷰포트에 표시
		HUDWidgetInstance->AddToViewport();

		// 마우스 커서 가리기, InputMode 게임Only 로 변경
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());

		ASpartaGameState* GameState = (GetWorld()) ?
			GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
		if (GameState)
		{
			GameState->UpdateHUD();
		}
	}

}

void ASpartaPlayerController::ShowMainMenu(bool bIsRestart)
{
	CloseWidget();

	if (!MainMenuWidgetClass) return;

	MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->AddToViewport();

		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());

		UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText")));
		if (ButtonText)
		{
			if (bIsRestart)
			{
				ButtonText->SetText(FText::FromString(TEXT("Restart")));
			}
			else
			{
				ButtonText->SetText(FText::FromString(TEXT("Start")));
			}
		}

		if (bIsRestart)
		{
			UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
			if (PlayAnimFunc)
			{
				MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
			}

			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("ScoreText"))))
			{
				if (USpartaGameInstance* SpartaGameInstance =
					Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TotalScoreText->SetText(FText::FromString(
						FString::Printf(TEXT("Total Score : %d"), SpartaGameInstance->GetTotalScore())
						));
				}
			}
		}
	}
}

void ASpartaPlayerController::StartGame()
{
	if (USpartaGameInstance* SpartaGameInstance =
		Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		SpartaGameInstance->Init();
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
	SetPause(false);
}

void ASpartaPlayerController::CloseWidget()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}
}
