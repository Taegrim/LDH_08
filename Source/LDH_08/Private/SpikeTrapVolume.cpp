#include "SpikeTrapVolume.h"

#include "Components/BoxComponent.h"

ASpikeTrapVolume::ASpikeTrapVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	SpawnBox->SetupAttachment(Scene);

	SpikeTrapCount = 10;
	bTraceToGround = true;
}

void ASpikeTrapVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DeactivateSpikeTraps();

	Super::EndPlay(EndPlayReason);
}

void ASpikeTrapVolume::ActivateSpikeTraps()
{
	DeactivateSpikeTraps();

	if (!SpikeTrapClass || !GetWorld()) return;

	for (int32 i = 0; i < SpikeTrapCount; ++i)
	{
		AActor* SpawnedTrap = GetWorld()->SpawnActor<AActor>(
			SpikeTrapClass,
			GetSpawnLocation(),
			FRotator::ZeroRotator
		);

		if (SpawnedTrap)
		{
			SpawnedSpikeTraps.Add(SpawnedTrap);
		}
	}
}

void ASpikeTrapVolume::DeactivateSpikeTraps()
{
	for (AActor* Trap : SpawnedSpikeTraps)
	{
		if (IsValid(Trap))
		{
			Trap->Destroy();
		}
	}

	SpawnedSpikeTraps.Empty();
}

FVector ASpikeTrapVolume::GetRandomLocationInVolume() const
{
	const FVector Origin = SpawnBox->GetComponentLocation();
	const FVector Extent = SpawnBox->GetScaledBoxExtent();

	return Origin + FVector(
		FMath::RandRange(-Extent.X, Extent.X),
		FMath::RandRange(-Extent.Y, Extent.Y),
		FMath::RandRange(-Extent.Z, Extent.Z)
	);
}

FVector ASpikeTrapVolume::GetSpawnLocation() const
{
	const FVector RandomLocation = GetRandomLocationInVolume();
	if (!bTraceToGround || !GetWorld()) return RandomLocation;

    const FVector TraceStart = RandomLocation + FVector(0.f, 0.f, 500.f);
	const FVector TraceEnd = RandomLocation - FVector(0.f, 0.f, 2000.f);

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility))
	{
		return HitResult.Location;
	}

	return RandomLocation;
}
