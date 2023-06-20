// Fill out your copyright notice in the Description page of Project Settings.

#include "CheckPoint.h"
#include "Kart.h"

// Sets default values
ACheckPoint::ACheckPoint()
{
	boxCollision = CreateDefaultSubobject<UBoxComponent>("boxCollision");
	RootComponent = boxCollision;
}

void ACheckPoint::BeginPlay() 
{
	boxCollision->OnComponentBeginOverlap.AddDynamic(this, &ACheckPoint::OnOverlapBegin);
}

void ACheckPoint::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	if (OtherActor->IsA(AKart::StaticClass()))
	{
		AKart* kart = Cast<AKart>(OtherActor);

		if (nextCheckPoint != nullptr)
		{
			kart->SetNextCheckPoint(this);
		}
	}
}