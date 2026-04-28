#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bombardment.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class LDH_08_API ABombardment : public AActor
{
	GENERATED_BODY()

public:
	ABombardment();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
    // 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bombardment|Component")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bombardment|Component")
	UStaticMeshComponent* WarningMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bombardment|Component")
	USphereComponent* Collision;

    // 파티클, 사운드
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bombardment|Effects")
    UParticleSystem* BombParticle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bombardment|Effects")
    USoundBase* BombSound;

    // 경고 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bombardment|Warning")
	float WarningDuration;

    // 포격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bombardment|Damage")
	float Damage;

	FTimerHandle StrikeTimerHandle;

	void Strike();
};
