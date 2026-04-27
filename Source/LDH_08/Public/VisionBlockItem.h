#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "VisionBlockItem.generated.h"

UCLASS()
class LDH_08_API AVisionBlockItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AVisionBlockItem();

	virtual void ActivateItem(AActor* Activator) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float BlockDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float VisionRestrictionStrength;
};
