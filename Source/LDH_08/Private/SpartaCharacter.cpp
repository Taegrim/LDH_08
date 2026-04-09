#include "SpartaCharacter.h"

#include "EnhancedInputComponent.h"
#include "SpartaPlayerController.h"
#include "Camera/CameraComponent.h"
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
	
	// 리플렉션 변수 디폴트 값으로 초기화
	NormalSpeed = 600.f;
	SprintMultiplier = 1.7f;
	RotateSpeed = 500.f;
	MaxHealth = 100;
	
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
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
		GetCharacterMovement()->RotationRate = FRotator(0.f, RotateSpeed, 0.f);
	}
}

void ASpartaCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Health healed : %f"), Health);
}

float ASpartaCharacter::GetHealth() const
{
	return Health;
}

void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Init();
}

float ASpartaCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Take Damaged, Current Hp : %f"), Health);
	
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
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void ASpartaCharacter::EndSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void ASpartaCharacter::OnDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Character is Death!!"));
	// 사망 처리
}

