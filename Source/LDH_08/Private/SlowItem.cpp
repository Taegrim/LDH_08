#include "SlowItem.h"

#include "SpartaCharacter.h"

ASlowItem::ASlowItem()
{
	ItemType = "Slow";
	SlowDuration = 5.f;
	MoveSpeedMultiplier = 0.5f;
}

void ASlowItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ASpartaCharacter* SpartaCharacter = Cast<ASpartaCharacter>(Activator))
		{
		    // 슬로우 타입의 효과를 적용
			SpartaCharacter->ApplyStatusEffect(
				EPlayerStatusEffectType::Slow,
				SlowDuration,
				MoveSpeedMultiplier
			);
		}
	}

	DestroyItem();
}
