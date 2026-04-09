#include "SpartaPlayerController.h"
#include "EnhancedInputSubsystems.h"

ASpartaPlayerController::ASpartaPlayerController() : InputMappingContext(nullptr),
                                                     MoveAction(nullptr), LookAction(nullptr), JumpAction(nullptr), SprintAction(nullptr)
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
