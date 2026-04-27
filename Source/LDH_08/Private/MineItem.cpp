#include "MineItem.h"

#include "Algo/ForEach.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AMineItem::AMineItem()
{
	ExplosionDamage = 35;
	ExplosionRadius = 300.f;
	ExplosionDelay = 5.f;
	ItemType = "Mine";
	bHasExploded = false;
	PickupParticle = nullptr;
	PickupSound = nullptr;

	ExplosionCollision = CreateDefaultSubobject<USphereComponent>("Explosion Collision");
	ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	ExplosionCollision->SetSphereRadius(ExplosionRadius);
	ExplosionCollision->SetupAttachment(Scene);
}

void AMineItem::ActivateItem(AActor* Activator)
{
	if (bHasExploded) return;

	Super::ActivateItem(Activator);

	GetWorldTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&AMineItem::Explode,
		ExplosionDelay,
		false
		);

	bHasExploded = true;
}

void AMineItem::Explode()
{
	UParticleSystemComponent* Particle = nullptr;

	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticle,
			GetActorLocation(),
			GetActorRotation(),
			false
			);
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation()
			);
	}

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

	if (Particle)
	{
		FTimerHandle DestroyParticleTimerHandle;
		TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[WeakParticle]()
			{
				if (WeakParticle.IsValid())
				{
					WeakParticle->DestroyComponent();
				}
			},
			2.f,
			false
		);
	}

	DestroyItem();
}

float AMineItem::GetPickupParticleLifeTime() const
{
	return ExplosionDelay;
}

void AMineItem::PlayPickupSound()
{
	if (PickupSound)
	{
		MineSound = UGameplayStatics::SpawnSoundAttached(
			PickupSound,
			GetRootComponent()
		);
	}
}

void AMineItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MineSound)
	{
		MineSound->Stop();
		MineSound = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}
