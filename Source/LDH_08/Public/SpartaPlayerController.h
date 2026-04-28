#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpartaPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UUserWidget;

UENUM(BlueprintType)
enum class EMenuState : uint8
{
    MainMenu    UMETA(DisplayName = "Main Menu"),
    Pause       UMETA(DisplayName = "Pause"),
    GameOver    UMETA(DisplayName = "Game Over")
};

UCLASS()
class LDH_08_API ASpartaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASpartaPlayerController();

	virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

	UInputAction* GetMoveAction() const;
	UInputAction* GetLookAction() const;
	UInputAction* GetJumpAction() const;
	UInputAction* GetSprintAction() const;

	UFUNCTION(BlueprintPure, Category = "HUD")
	UUserWidget* GetHUDWidget() const;
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowGameHUD();		// HUD 표시

    UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowMainMenu(EMenuState MenuState);	// 메뉴 표시
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void OnStartButtonClicked();
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void ReturnToMainMenu();
    UFUNCTION(BlueprintCallable, Category = "Menu")
	void StartGame();
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void ResumeGame();
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void ToggleMenu();

protected:
	// IMC
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;

	// IA
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MenuAction;

	// 위젯 클래스 자체를 저장하는 변수와, 인스턴스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
	UUserWidget* HUDWidgetInstance;

	// 메뉴 클래스 저장하는 변수, 인스턴스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
	UUserWidget* MainMenuWidgetInstance;

    // 현재 메뉴창 상태 저장
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
    EMenuState CurrentMenuState;

private:
	void CloseWidget();		// 기존 UI 인스턴스 제거
    void ApplyMenuStateToWidget();
    bool IsInMenuLevel() const;
};

