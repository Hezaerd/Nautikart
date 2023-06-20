// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Path.h"
#include "Components/BoxComponent.h"
#include "Kart.h"

// Sets default values
APath::APath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>("root");
	RootComponent = root;

	path = CreateDefaultSubobject<USplineComponent>("spline");
	path->SetupAttachment(root);

	boxCollider = CreateDefaultSubobject<UBoxComponent>("boxCollider");
	boxCollider->SetCollisionProfileName("OverlapAllDynamic");
	boxCollider->OnComponentBeginOverlap.AddDynamic(this, &APath::OnOverlapBegin);
}

// Called when the game starts or when spawned
void APath::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APath::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (OtherActor->IsA(AKart::StaticClass()))
		{
			AKart* kart = Cast<AKart>(OtherActor);

			if (bIsNextLoop)
			{
				kart->ShouldActivateLoopPath(true);
			}
			else
			{
				kart->SetActivePath(nextPath);
			}

			if (bWasPathLoop)
			{
				kart->ShouldActivateLoopPath(false);
			}
		}
	}
}

USplineComponent* APath::GetSplineComponent()
{
	return path;
}

