#include "Bombardment.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

ABombardment::ABombardment()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	WarningMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WarningMesh"));
	WarningMesh->SetupAttachment(Scene);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetupAttachment(Scene);
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	WarningDuration = 1.f;
	Damage = 30.f;
    BombParticle = nullptr;
    BombSound = nullptr;
}

void ABombardment::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(
		StrikeTimerHandle,
		this,
		&ABombardment::Strike,
		WarningDuration,
		false
	);
}

void ABombardment::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(StrikeTimerHandle);

	Super::EndPlay(EndPlayReason);
}

void ABombardment::Strike()
{
    if (BombParticle)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            BombParticle,
            GetActorLocation(),
            GetActorRotation()
            );
    }

    if (BombSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            BombSound,
            GetActorLocation()
            );
    }

	TArray<AActor*> OverlappingActors;
	Collision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag(TEXT("Player")))
		{
			UGameplayStatics::ApplyDamage(Actor, Damage, nullptr, this, UDamageType::StaticClass());
		}
	}

    Destroy();
}
