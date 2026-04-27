#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class UWidgetComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;


// 효과 종류
UENUM(BlueprintType)
enum class EPlayerStatusEffectType : uint8
{
	Slow                UMETA(DisplayName = "Slow", ToolTip = "Slow Multiplier"),
	VisionRestriction   UMETA(DisplayName = "Vision Restriction", ToolTip = "Vision Restriction Multiplier")
};

// 효과 정보 구조체
USTRUCT(BlueprintType)
struct FActiveStatusEffect
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect")
	EPlayerStatusEffectType Type = EPlayerStatusEffectType::Slow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect")
	float RemainingTime = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect")
	float Duration = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect")
	float Strength = 0.f;
};

UCLASS()
class LDH_08_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpartaCharacter();

	void Init();
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Effect")
	void ApplyStatusEffect(EPlayerStatusEffectType EffectType, float Duration, float Strength);
	UFUNCTION(BlueprintPure, Category = "Effect")
	TArray<FActiveStatusEffect> GetActiveStatusEffects() const;
	UFUNCTION(BlueprintPure, Category = "Effect")
	int32 GetStatusEffectStackCount(EPlayerStatusEffectType EffectType) const;
	UFUNCTION(BlueprintPure, Category = "Effect")
	float GetLongestStatusEffectRemainingTime(EPlayerStatusEffectType EffectType) const;
    UFUNCTION(BlueprintCallable, Category = "Effect")
    void StartStatusEffectTimer();
    UFUNCTION(BlueprintCallable, Category = "Effect")
    void ResetStatusEffects();

	virtual void BeginPlay() override;
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Conponent")
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Conponent")
	UCameraComponent* CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;

	// 리플렉션 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float NormalSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float RotateSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float Health;

	float SprintSpeed;
	bool bIsSprinting;                      // 달리는 상태인지 확인
	float CurrentMoveSpeedMultiplier;       // 이동속도 배율 (슬로우 구현을 위한 곱)

    FTimerHandle StatusEffectTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect")
	TArray<FActiveStatusEffect> ActiveStatusEffects;    // 적용중인 효과

    // Input 에 따른 처리 함수
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void EndJump(const FInputActionValue& Value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& Value);
	UFUNCTION()
	void EndSprint(const FInputActionValue& Value);

	void OnDeath();
	void UpdateOverheadHP();
	void UpdateStatusEffects();
	void CalculateStatusEffects();
	void UpdateMovementSpeed();
	void UpdateStatusEffectHUD();

    // 블루프린트 전용 함수
	UFUNCTION(BlueprintImplementableEvent, Category = "Effect")
	void OnStatusEffectsChanged(const TArray<FActiveStatusEffect>& StatusEffects);

private:
    const float StatusEffectTickInterval = 0.1f;
};
