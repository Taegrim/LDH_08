#include "SpartaPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SpartaGameInstance.h"
#include "SpartaGameState.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"

ASpartaPlayerController::ASpartaPlayerController() :
	InputMappingContext(nullptr), MoveAction(nullptr), LookAction(nullptr), JumpAction(nullptr), SprintAction(nullptr),
    MenuAction(nullptr), HUDWidgetClass(nullptr), HUDWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr), MainMenuWidgetInstance(nullptr),
    CurrentMenuState(EMenuState::MainMenu)
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
    if (IsInMenuLevel())
    {
		ShowMainMenu(EMenuState::MainMenu);
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

		ASpartaGameState* GameState = (GetWorld()) ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
		if (GameState)
		{
			GameState->UpdateHUD();
		}
	}
}

void ASpartaPlayerController::ShowMainMenu(EMenuState MenuState)
{
	CloseWidget();

	if (!MainMenuWidgetClass) return;

    CurrentMenuState = MenuState;

	MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->AddToViewport();

	    // 커서 보이게
		bShowMouseCursor = true;

	    if (MenuState == EMenuState::Pause)
	    {
	        SetPause(true);

	        // UI 처리하면서 키보드 입력도 받을 수 있게함
	        // 입력 포커스는 MainMenu Widget에 두고, 마우스는 가두지 않음
	        FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(MainMenuWidgetInstance->TakeWidget());
	        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	        SetInputMode(InputMode);
	    }
	    else
	    {
		    SetInputMode(FInputModeUIOnly());
	    }

	    ApplyMenuStateToWidget();
	}
}

void ASpartaPlayerController::OnStartButtonClicked()
{
    switch (CurrentMenuState)
    {
    case EMenuState::MainMenu:
        StartGame();
        break;

    case EMenuState::Pause:
        ResumeGame();
        break;

    case EMenuState::GameOver:
        StartGame();
        break;
    }
}

void ASpartaPlayerController::ReturnToMainMenu()
{
    SetPause(false);

    if (USpartaGameInstance* SpartaGameInstance =
        Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        SpartaGameInstance->Init();
    }

	UGameplayStatics::OpenLevel(GetWorld(), FName("MenuLevel"));
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

void ASpartaPlayerController::ResumeGame()
{
    ShowGameHUD();
    SetPause(false);
}

void ASpartaPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (MenuAction)
        {
            EnhancedInput->BindAction(
                MenuAction,
                ETriggerEvent::Started,
                this,
                &ASpartaPlayerController::ToggleMenu);
        }
    }
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

void ASpartaPlayerController::ApplyMenuStateToWidget()
{
    if (!MainMenuWidgetInstance) return;

    UTextBlock* TitleText =
        Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("TitleText")));

    UTextBlock* ScoreText =
        Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("ScoreText")));

    UTextBlock* StartButtonText =
        Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText")));

    UWidget* MainMenuButton =
        MainMenuWidgetInstance->GetWidgetFromName(TEXT("MenuButton"));

    switch (CurrentMenuState)
    {
    case EMenuState::MainMenu:
        if (TitleText)
        {
            TitleText->SetVisibility(ESlateVisibility::Hidden);
        }
        if (ScoreText)
        {
            ScoreText->SetVisibility(ESlateVisibility::Hidden);
        }
        if (StartButtonText)
        {
            StartButtonText->SetText(FText::FromString(TEXT("Start")));
        }
        if (MainMenuButton)
        {
            MainMenuButton->SetVisibility(ESlateVisibility::Hidden);
        }
        break;

    case EMenuState::Pause:
        if (TitleText)
        {
            TitleText->SetVisibility(ESlateVisibility::HitTestInvisible);
            TitleText->SetText(FText::FromString(TEXT("Paused")));
        }
        if (ScoreText)
        {
            ScoreText->SetVisibility(ESlateVisibility::Hidden);
        }
        if (StartButtonText)
        {
            StartButtonText->SetText(FText::FromString(TEXT("Resume")));
        }
        if (MainMenuButton)
        {
            MainMenuButton->SetVisibility(ESlateVisibility::Visible);
        }
        break;

    case EMenuState::GameOver:
        if (TitleText)
        {
            TitleText->SetVisibility(ESlateVisibility::HitTestInvisible);
            TitleText->SetText(FText::FromString(TEXT("Game Over!")));
        }
        if (ScoreText)
        {
            ScoreText->SetVisibility(ESlateVisibility::HitTestInvisible);

            if (USpartaGameInstance* SpartaGameInstance =
                Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
            {
                ScoreText->SetText(FText::FromString(
                    FString::Printf(TEXT("Total Score : %d"), SpartaGameInstance->GetTotalScore())
                ));
            }
        }
        if (StartButtonText)
        {
            StartButtonText->SetText(FText::FromString(TEXT("Restart")));
        }
        if (MainMenuButton)
        {
            MainMenuButton->SetVisibility(ESlateVisibility::Visible);
        }
        break;
    }
}

bool ASpartaPlayerController::IsInMenuLevel() const
{
    const FString MapName = GetWorld() ? GetWorld()->GetMapName() : TEXT("");
    return MapName.Contains(TEXT("MenuLevel"));
}

void ASpartaPlayerController::ToggleMenu()
{
    if (IsInMenuLevel()) return;

    if (CurrentMenuState == EMenuState::Pause && MainMenuWidgetInstance)
    {
        ResumeGame();
    }
    else
    {
        ShowMainMenu(EMenuState::Pause);
    }
}
