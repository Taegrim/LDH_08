#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SlowItem.generated.h"

UCLASS()
class LDH_08_API ASlowItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ASlowItem();

	virtual void ActivateItem(AActor* Activator) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float SlowDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float MoveSpeedMultiplier;
};
