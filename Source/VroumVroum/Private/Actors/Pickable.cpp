// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Pickable.h"

#include "AI/P_AI.h"
#include "Kart.h"
#include "Kismet/GameplayStatics.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APickable::APickable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!BoxCollider)
	{
		// Create a box collider
		BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));

		BoxCollider->SetCollisionProfileName("OverlapAllDynamic");
		BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &APickable::OnOverlapBegin);

		RootComponent = BoxCollider;
	}

	if (!Mesh)
	{
		// Create a mesh component
		Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

		Mesh->SetCollisionProfileName("NoCollision");
		Mesh->SetupAttachment(BoxCollider);
	}
}

// Called when the game starts or when spawned
void APickable::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APickable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// On Overlap Begin
void APickable::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// If the overlapped actor is the player
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (OtherActor->IsA(AKart::StaticClass()))
		{
			UGameplayStatics::PlaySound2D(UGameplayStatics::GetPlayerPawn(this, 0), pickUpSound);

			AKart* kart = Cast<AKart>(OtherActor);

			kart->AddItem(GenerateRandomItem());

			if (OtherActor->IsA(AP_AI::StaticClass()))
			{
				AP_AI* ai = Cast<AP_AI>(OtherActor);

				ai->DelayUseItem();
			}

			Disable();
		}
	}
}

EItems APickable::GenerateRandomItem()
{
	int32 random = FMath::RandRange(0, 3);

	switch (random)
	{
	case 0:
		return EItems::GreenShell;
		break;
	case 1:
		return EItems::Banana;
		break;
	case 2:
		return EItems::Boost;
		break;
	case 3:
		return EItems::Bomb;
		break;
	default:
		return EItems::NoItem;
		break;
	}
}

void APickable::Disable()
{
	bIsEnable = false;

	// Disable the mesh
	Mesh->SetVisibility(false);

	// Disable the collider
	BoxCollider->SetCollisionProfileName("NoCollision");

	GetWorldTimerManager().SetTimer(respawnTimer, this, &APickable::Enable, respawnTime, false);
}

void APickable::Enable()
{
	bIsEnable = true;

	// Disable the mesh
	Mesh->SetVisibility(true);

	// Disable the collider
	BoxCollider->SetCollisionProfileName("OverlapAllDynamic");

	GetWorldTimerManager().ClearTimer(respawnTimer);
}