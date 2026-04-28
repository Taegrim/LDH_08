#include "BombardmentVolume.h"

#include "Components/BoxComponent.h"

ABombardmentVolume::ABombardmentVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	SpawnBox->SetupAttachment(Scene);

	SpawnInterval = 5.f;
    BombardmentCount = 5;
	bTraceToGround = true;
}

void ABombardmentVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopBombardment();

	Super::EndPlay(EndPlayReason);
}

void ABombardmentVolume::StartBombardment()
{
    // 기존 폭격 정리 후 생성
	StopBombardment();
	SpawnBombardment();

	GetWorldTimerManager().SetTimer(
		BombardmentTimerHandle,
		this,
		&ABombardmentVolume::SpawnBombardment,
		SpawnInterval,
		true
	);
}

void ABombardmentVolume::StopBombardment()
{
	GetWorldTimerManager().ClearTimer(BombardmentTimerHandle);

	for (AActor* Bombardment : SpawnedBombardments)
	{
		if (IsValid(Bombardment))
		{
			Bombardment->Destroy();
		}
	}

	SpawnedBombardments.Empty();
}

void ABombardmentVolume::SpawnBombardment()
{
	if (!BombardmentClass || !GetWorld()) return;

    for (int32 i = 0; i < BombardmentCount; ++i)
    {
        AActor* SpawnedBombardment = GetWorld()->SpawnActor<AActor>(
            BombardmentClass,
            GetSpawnLocation(),
            FRotator::ZeroRotator
        );

        if (SpawnedBombardment)
        {
            SpawnedBombardments.Add(SpawnedBombardment);
        }
    }
}

FVector ABombardmentVolume::GetRandomLocationInVolume() const
{
	const FVector Origin = SpawnBox->GetComponentLocation();
	const FVector Extent = SpawnBox->GetScaledBoxExtent();

	return Origin + FVector(
		FMath::RandRange(-Extent.X, Extent.X),
		FMath::RandRange(-Extent.Y, Extent.Y),
		FMath::RandRange(-Extent.Z, Extent.Z)
	);
}

FVector ABombardmentVolume::GetSpawnLocation() const
{
	const FVector RandomLocation = GetRandomLocationInVolume();
	if (!bTraceToGround || !GetWorld()) return RandomLocation;

    // 바닥에 폭격 처리
	const FVector TraceStart = RandomLocation + FVector(0.f, 0.f, 500.f);
	const FVector TraceEnd = RandomLocation - FVector(0.f, 0.f, 2000.f);

    // 바닥에 트레이스 성공하면 해당 좌표 반환
	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility))
	{
		return HitResult.Location;
	}

    // 실패시 생성된 위치 그대로 반환
	return RandomLocation;
}
