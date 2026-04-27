#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "HealingItem.generated.h"

UCLASS()
class LDH_08_API AHealingItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AHealingItem();

	virtual void ActivateItem(AActor* Activator) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float HealAmount;
};
