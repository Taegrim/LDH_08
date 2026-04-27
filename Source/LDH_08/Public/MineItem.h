#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "MineItem.generated.h"

UCLASS()
class LDH_08_API AMineItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AMineItem();

	virtual void ActivateItem(AActor* Activator) override;

protected:
	UPROPERTY()
	UAudioComponent* MineSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USphereComponent* ExplosionCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ExplosionDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ExplosionDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* ExplosionParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* ExplosionSound;

	bool bHasExploded;
	FTimerHandle ExplosionTimerHandle;

	void Explode();

	virtual float GetPickupParticleLifeTime() const override;

	virtual void PlayPickupSound() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
