#include "VisionBlockItem.h"

#include "SpartaCharacter.h"

AVisionBlockItem::AVisionBlockItem()
{
	ItemType = "VisionBlock";
	BlockDuration = 5.f;
	VisionRestrictionStrength = 1.f;
}

void AVisionBlockItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

    if (Activator && Activator->ActorHasTag("Player"))
    {
        if (ASpartaCharacter* SpartaCharacter = Cast<ASpartaCharacter>(Activator))
        {
            SpartaCharacter->ApplyStatusEffect(
                EPlayerStatusEffectType::VisionRestriction,
                BlockDuration,
                VisionRestrictionStrength
            );
        }
    }

	DestroyItem();
}
