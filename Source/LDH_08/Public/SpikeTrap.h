#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpikeTrap.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class LDH_08_API ASpikeTrap : public AActor
{
	GENERATED_BODY()

public:
	ASpikeTrap();

protected:
    //컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spike Trap|Component")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spike Trap|Component")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spike Trap|Component")
	UBoxComponent* Collision;

    // 충돌 데미지 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Trap|Dagame")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Trap|Damage")
	float DamageCooldown;

	TMap<TWeakObjectPtr<AActor>, float> LastDamageTimes;

	UFUNCTION()
	void OnTrapOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

};
