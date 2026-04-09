#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "CoinItem.generated.h"

UCLASS()
class LDH_08_API ACoinItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ACoinItem();
	
	virtual void ActivateItem(AActor* Activator) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 PointValue;
};
