// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplaceTrigger.h"
#include "Kart.h"

// Sets default values
AReplaceTrigger::AReplaceTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxCollider = CreateDefaultSubobject<UBoxComponent>("boxCollider");
	RootComponent = boxCollider;
}

// Called when the game starts or when spawned
void AReplaceTrigger::BeginPlay()
{

	boxCollider->bHiddenInGame = bBoxHiddenInGame;

	boxCollider->OnComponentBeginOverlap.AddDynamic(this, &AReplaceTrigger::OnOverlapBegin);
}


void AReplaceTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (OtherActor->IsA(AKart::StaticClass()))
		{
			AKart* kart = Cast<AKart>(OtherActor);

			kart->ReplaceKart();
		}
	}
}