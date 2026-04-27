#include "SpartaCharacter.h"

#include "EnhancedInputComponent.h"
#include "SpartaGameState.h"
#include "SpartaPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

    // 변수 초기화
	NormalSpeed = 600.f;
	SprintMultiplier = 1.7f;
	RotateSpeed = 500.f;
	MaxHealth = 100;
	bIsSprinting = false;
	CurrentMoveSpeedMultiplier = 1.f;
	ActiveStatusEffects.Empty();

    // 캐릭터 이동방향으로 회전
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}

	Init();
}

void ASpartaCharacter::Init()
{
    // 리플렉션에 의해 변하는 변수들을 BeginPlay에서 다시 초기화
	SprintSpeed = NormalSpeed * SprintMultiplier;
	Health = MaxHealth;
	bIsSprinting = false;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->RotationRate = FRotator(0.f, RotateSpeed, 0.f);
	}

	UpdateMovementSpeed();
}

void ASpartaCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.f, MaxHealth);
	UpdateOverheadHP();
}

float ASpartaCharacter::GetHealth() const
{
	return Health;
}

// 캐릭터에 효과 적용
void ASpartaCharacter::ApplyStatusEffect(EPlayerStatusEffectType EffectType, float Duration, float Strength)
{
	if (Duration <= 0.f) return;

	FActiveStatusEffect Effect{};
	Effect.Type = EffectType;
	Effect.Duration = Duration;
	Effect.RemainingTime = Duration;
	Effect.Strength = Strength;

    // 인자로 받은 상태를 현재 적용중인 상태 배열에 추가
	ActiveStatusEffects.Add(Effect);

    // 상태 효과 계산, 블루 프린트 이벤트 발생
    // 효과 HUD 갱신, 타이머 시작
	CalculateStatusEffects();
	OnStatusEffectsChanged(ActiveStatusEffects);
	UpdateStatusEffectHUD();
    StartStatusEffectTimer();
}

TArray<FActiveStatusEffect> ASpartaCharacter::GetActiveStatusEffects() const
{
	return ActiveStatusEffects;
}

// 특정 타입의 효과가 몇 중첩인지 반환하는 함수
int32 ASpartaCharacter::GetStatusEffectStackCount(EPlayerStatusEffectType EffectType) const
{
	int32 StackCount = 0;

	for (const FActiveStatusEffect& Effect : ActiveStatusEffects)
	{
		if (Effect.Type == EffectType)
		{
			++StackCount;
		}
	}

	return StackCount;
}

// 특정 효과의 가장 긴 남은 시간 반환하는 함수
float ASpartaCharacter::GetLongestStatusEffectRemainingTime(EPlayerStatusEffectType EffectType) const
{
	float LongestRemainingTime = 0.f;

	for (const FActiveStatusEffect& Effect : ActiveStatusEffects)
	{
		if (Effect.Type == EffectType)
		{
			LongestRemainingTime = FMath::Max(LongestRemainingTime, Effect.RemainingTime);
		}
	}

	return LongestRemainingTime;
}

// 효과 타이머 시작하는 함수
void ASpartaCharacter::StartStatusEffectTimer()
{
    // 기존에 타이머가 돌고 있으면 return
    if (GetWorldTimerManager().IsTimerActive(StatusEffectTimerHandle))
    {
        return;
    }

    GetWorldTimerManager().SetTimer(
        StatusEffectTimerHandle,
        this,
        &ASpartaCharacter::UpdateStatusEffects,
        StatusEffectTickInterval,
        true
        );
}

void ASpartaCharacter::ResetStatusEffects()
{
    GetWorldTimerManager().ClearTimer(StatusEffectTimerHandle);

    ActiveStatusEffects.Empty();
    CurrentMoveSpeedMultiplier = 1.f;

    UpdateMovementSpeed();
    OnStatusEffectsChanged(ActiveStatusEffects);
	UpdateStatusEffectHUD();
}

void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();

	Init();

	UpdateOverheadHP();
}

float ASpartaCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
	UpdateOverheadHP();

	if (Health <= 0.f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Enhanced Input Component 로 캐스팅
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput) return;

    // 현재 소유중인 컨트롤러를 SpartaController로 캐스팅
    // IA들을 가져오기 위함
	ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController());
	if (!PlayerController) return;

	if (PlayerController->GetMoveAction())
	{
		EnhancedInput->BindAction(PlayerController->GetMoveAction(), ETriggerEvent::Triggered, this,
			&ASpartaCharacter::Move);
	}
	if (PlayerController->GetLookAction())
	{
		EnhancedInput->BindAction(PlayerController->GetLookAction(), ETriggerEvent::Triggered, this,
			&ASpartaCharacter::Look);
	}
	if (PlayerController->GetJumpAction())
	{
		EnhancedInput->BindAction(PlayerController->GetJumpAction(), ETriggerEvent::Triggered, this,
			&ASpartaCharacter::StartJump);
		EnhancedInput->BindAction(PlayerController->GetJumpAction(), ETriggerEvent::Completed, this,
			&ASpartaCharacter::EndJump);
	}
	if (PlayerController->GetSprintAction())
	{
		EnhancedInput->BindAction(PlayerController->GetSprintAction(), ETriggerEvent::Triggered, this,
			&ASpartaCharacter::StartSprint);
		EnhancedInput->BindAction(PlayerController->GetSprintAction(), ETriggerEvent::Completed, this,
			&ASpartaCharacter::EndSprint);
	}
}

void ASpartaCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	FVector2D MoveInput = Value.Get<FVector2D>();

	FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, MoveInput.Y);
	}
}

void ASpartaCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ASpartaCharacter::StartJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		Jump();
	}
}

void ASpartaCharacter::EndJump(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		StopJumping();
	}
}

void ASpartaCharacter::StartSprint(const FInputActionValue& Value)
{
	bIsSprinting = true;
	UpdateMovementSpeed();
}

void ASpartaCharacter::EndSprint(const FInputActionValue& Value)
{
	bIsSprinting = false;
	UpdateMovementSpeed();
}

void ASpartaCharacter::OnDeath()
{
	ASpartaGameState* GameState = (GetWorld()) ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if (GameState)
	{
		GameState->OnGameOver();
	}
}

void ASpartaCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverheadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}
}

// 효과 업데이트 함수
void ASpartaCharacter::UpdateStatusEffects()
{
	if (ActiveStatusEffects.IsEmpty()) return;

	bool bChanged = false;

    // RemoveAt 시 원소들이 앞으로 당겨지므로 뒤에서부터 탐색
	for (int32 i = ActiveStatusEffects.Num() - 1; i >= 0; --i)
	{
		ActiveStatusEffects[i].RemainingTime -= StatusEffectTickInterval;

		if (ActiveStatusEffects[i].RemainingTime <= 0.f)
		{
			ActiveStatusEffects.RemoveAt(i);
			bChanged = true;
		}
	}

    // 변경된 값이 있다면 상태 계산
	if (bChanged)
	{
		CalculateStatusEffects();
	}

	OnStatusEffectsChanged(ActiveStatusEffects);
	UpdateStatusEffectHUD();

    // 만약 모든 효과가 끝나면 타이머 해제
    if (ActiveStatusEffects.IsEmpty())
    {
        GetWorldTimerManager().ClearTimer(StatusEffectTimerHandle);
    }
}

void ASpartaCharacter::CalculateStatusEffects()
{
	CurrentMoveSpeedMultiplier = 1.f;

	for (const FActiveStatusEffect& Effect : ActiveStatusEffects)
	{
		if (Effect.Type == EPlayerStatusEffectType::Slow)
		{
			CurrentMoveSpeedMultiplier *= FMath::Clamp(Effect.Strength, 0.f, 1.f);
		}
	}

	UpdateMovementSpeed();
}

// 이동속도 변환, 달리는 상태인지 확인 후 최종 속도에 현재 속도 변환값 곱셈을 함
void ASpartaCharacter::UpdateMovementSpeed()
{
	if (!GetCharacterMovement()) return;

	const float Speed = bIsSprinting ? SprintSpeed : NormalSpeed;
	GetCharacterMovement()->MaxWalkSpeed = Speed * CurrentMoveSpeedMultiplier;
}

void ASpartaCharacter::UpdateStatusEffectHUD()
{
	ASpartaPlayerController* SpartaController = Cast<ASpartaPlayerController>(GetController());
	if (!SpartaController) return;

	UUserWidget* HUDWidget = SpartaController->GetHUDWidget();
	if (!HUDWidget) return;

	if (UTextBlock* SlowEffectText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("SlowEffectText"))))
	{
	    const int32 SlowStack = GetStatusEffectStackCount(EPlayerStatusEffectType::Slow);
	    const float SlowRemainingTime = GetLongestStatusEffectRemainingTime(EPlayerStatusEffectType::Slow);

		if (SlowStack > 0)
		{
		    // 보이지만 마우스 상호작용 대상에선 제외
			SlowEffectText->SetVisibility(ESlateVisibility::HitTestInvisible);
			SlowEffectText->SetText(FText::FromString(
				FString::Printf(TEXT("Slow Stack : %d, Time : %.1f"), SlowStack, SlowRemainingTime)
			));
		}
		else
		{
			SlowEffectText->SetVisibility(ESlateVisibility::Hidden);
		}
	}


	if (UTextBlock* VisionEffectText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("VisionEffectText"))))
	{
	    const int32 VisionStack = GetStatusEffectStackCount(EPlayerStatusEffectType::VisionRestriction);
	    const float VisionRemainingTime = GetLongestStatusEffectRemainingTime(EPlayerStatusEffectType::VisionRestriction);

		if (VisionStack > 0)
		{
			VisionEffectText->SetVisibility(ESlateVisibility::HitTestInvisible);
			VisionEffectText->SetText(FText::FromString(
				FString::Printf(TEXT("Vision Stack : %d, Time : %.1f"), VisionStack, VisionRemainingTime)
			));
		}
		else
		{
			VisionEffectText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

