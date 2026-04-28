#include "SpikeTrap.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"

ASpikeTrap::ASpikeTrap()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Scene);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(Scene);
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    // 오버랩 이벤트 바인딩
    Collision->OnComponentBeginOverlap.AddDynamic(this, &ASpikeTrap::OnTrapOverlap);

	Damage = 20.f;
	DamageCooldown = 1.f;
}

void ASpikeTrap::OnTrapOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || !OtherActor->ActorHasTag(TEXT("Player"))) return;

	const float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	const TWeakObjectPtr<AActor> OverlappedActor(OtherActor);
	const float* LastDamageTime = LastDamageTimes.Find(OverlappedActor);
	if (LastDamageTime && CurrentTime - *LastDamageTime < DamageCooldown) return;

	LastDamageTimes.Add(OverlappedActor, CurrentTime);
	UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, UDamageType::StaticClass());
}
