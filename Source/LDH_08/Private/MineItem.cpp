#include "MineItem.h"

#include "Algo/ForEach.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AMineItem::AMineItem()
{
	ExplosionDamage = 35;
	ExplosionRadius = 300.f;
	ExplosionDelay = 5.f;
	ItemType = "Mine";
	
	ExplosionCollision = CreateDefaultSubobject<USphereComponent>("Explosion Collision");
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionCollision->SetSphereRadius(ExplosionRadius);
	ExplosionCollision->SetupAttachment(Scene);
}

void AMineItem::ActivateItem(AActor* Activator)
{
	GetWorldTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&AMineItem::Explode,
		ExplosionDelay,
		false
		);
}

void AMineItem::Explode()
{
	TArray<AActor*> OverlappingActors;
	ExplosionCollision->GetOverlappingActors(OverlappingActors);
	
	Algo::ForEach(OverlappingActors,[this](AActor* Actor)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(
				Actor,
				ExplosionDamage,
				nullptr,
				this,
				UDamageType::StaticClass()
				);
		}
	});
	
	DestroyItem();
}
