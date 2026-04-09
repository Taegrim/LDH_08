#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Algo/Accumulate.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	SetRootComponent(Scene);
	
	SpawnBox = CreateDefaultSubobject<UBoxComponent>("SpawnBox");
	SpawnBox->SetupAttachment(Scene);
	
	ItemDataTable = nullptr;
}

AActor* ASpawnVolume::SpawnRandomItem()
{
	if (FSpawnItemRow* Row = GetRandomItem())
	{
		if (UClass* ActualClass = Row->ItemClass.Get())
		{
			return SpawnItem(ActualClass);
		}
	}
	return nullptr;
}

FSpawnItemRow* ASpawnVolume::GetRandomItem() const
{
	if (!ItemDataTable) return nullptr;
	
	TArray<FSpawnItemRow*> AllRows;
	static const FString ContextString(TEXT("Item Context"));
	
	ItemDataTable->GetAllRows(ContextString, AllRows);
	if (AllRows.IsEmpty()) return nullptr;
	
	// 총 확률 누적
	const float TotalChance = Algo::Accumulate(AllRows, 0.f, [](float Acc, const FSpawnItemRow* Row)
	{
		return Acc + (Row ? Row->SpawnChance : 0.f);
	});
	
	// 누적합으로 확률 뽑기 처리
	const float RandValue = FMath::RandRange(0.f, TotalChance);
	float AccumulateChance = 0.f;
	
	// Find If 와 유사한 FindByPredicate 로 처리
	FSpawnItemRow** FoundRow = AllRows.FindByPredicate([&](const FSpawnItemRow* Row)
	{
		if (!Row) return false;
		AccumulateChance += Row->SpawnChance;
		return RandValue <= AccumulateChance;
	});
	
	return FoundRow ? *FoundRow : nullptr;
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;
	
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ItemClass,	
		GetRandomSpawnLocation(),
		FRotator::ZeroRotator
		);
	
	return SpawnedActor;
}

FVector ASpawnVolume::GetRandomSpawnLocation() const
{
	FVector Origin = SpawnBox->GetComponentLocation();
	FVector Extent = SpawnBox->GetScaledBoxExtent();
	
	return Origin + FVector(
		FMath::RandRange(-Extent.X, Extent.X),
		FMath::RandRange(-Extent.Y, Extent.Y),
		FMath::RandRange(-Extent.Z, Extent.Z)
	);
}
