// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Bomb.h"
#include "Kismet/GameplayStatics.h"
#include "Kart.h"

ABomb::ABomb()
{
	PrimaryActorTick.bCanEverTick = true;

	if (!root)
	{
		root = CreateDefaultSubobject<USceneComponent>("Root");
		RootComponent = root;
	}

	if (!sphereCollider)
	{
		sphereCollider = CreateDefaultSubobject<USphereComponent>("SphereCollider");
		sphereCollider->SetupAttachment(root);

		sphereCollider->SetNotifyRigidBodyCollision(true);
		sphereCollider->SetCollisionProfileName("BlockAllDynamic");
	}

	if (!mesh)
	{
		mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
		mesh->SetupAttachment(sphereCollider);

		mesh->SetCollisionProfileName("NoCollision");
	}
}

void ABomb::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AKart* kart = Cast<AKart>(OtherActor);
	if (kart)
	{
		kart->GetHit();
		if (!bExplode)
			Explode();
		return;
	}
	AItem* item = Cast<AItem>(OtherActor);
	if (item)
	{
		if (!bExplode)
			Explode();
	}
}

void ABomb::BeginPlay()
{
	Super::BeginPlay();

	startScale = sphereCollider->GetComponentScale();

	sphereCollider->OnComponentHit.AddDynamic(this, &ABomb::OnHit);
	GetWorldTimerManager().SetTimer(timerHandle, this, &ABomb::Explode, timeToExplode, false);
}

void ABomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bExplode)
	{
		explosionRatio += DeltaTime;
		sphereCollider->SetWorldScale3D(FMath::Lerp(startScale, startScale * explosionRadius, explosionRatio / explosionTime));
	}
}

void ABomb::Explode()
{
	GetWorldTimerManager().ClearTimer(timerHandle);

	bExplode = true;
	UGameplayStatics::PlaySoundAtLocation(this, destroySound, mesh->GetComponentLocation(), mesh->GetComponentRotation());

	GetWorldTimerManager().SetTimer(timerHandle, this, &ABomb::DestroyBomb, explosionTime + 0.01f, false);
}

void ABomb::DestroyBomb()
{
	GetWorldTimerManager().ClearTimer(timerHandle);

	Destroy();
}