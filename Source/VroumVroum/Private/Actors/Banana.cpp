// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Banana.h"
#include "Kismet/GameplayStatics.h"
#include "Kart.h"

ABanana::ABanana()
{
	PrimaryActorTick.bCanEverTick = false;

	if (!root)
	{
		root = CreateDefaultSubobject<USceneComponent>("Root");
		RootComponent = root;
	}

	if (!capsuleCollider)
	{
		capsuleCollider = CreateDefaultSubobject<UCapsuleComponent>("CapsuleCollider");

		capsuleCollider->SetupAttachment(root);

		// Generate Hit event
		capsuleCollider->SetNotifyRigidBodyCollision(true);

		capsuleCollider->SetCollisionProfileName("BlockAllDynamic");
	}

	if (!mesh)
	{
		mesh = CreateDefaultSubobject<USceneComponent>("Mesh");

		mesh->SetupAttachment(capsuleCollider);
	}
}

void ABanana::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AKart* kart = Cast<AKart>(OtherActor);
	if (kart)
	{
		kart->GetHit();
		UGameplayStatics::PlaySoundAtLocation(this, destroySound, mesh->GetComponentLocation(), mesh->GetComponentRotation());
		Destroy();
		return;
	}
	AItem* item = Cast<AItem>(OtherActor);
	if (item)
	{
		UGameplayStatics::PlaySoundAtLocation(this, destroySound, mesh->GetComponentLocation(), mesh->GetComponentRotation());
		Destroy();
	}
}

void ABanana::BeginPlay()
{
	Super::BeginPlay();

	if (capsuleCollider)
		capsuleCollider->OnComponentHit.AddDynamic(this, &ABanana::OnHit);
}

void ABanana::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABanana::CurvedSpawn()
{
}