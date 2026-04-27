#include "HealingItem.h"
#include "SpartaCharacter.h"

AHealingItem::AHealingItem()
{
	HealAmount = 20.f;
	ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ASpartaCharacter* SpartaCharacter = Cast<ASpartaCharacter>(Activator))
		{
			SpartaCharacter->AddHealth(HealAmount);
		}

		DestroyItem();
	}
}
