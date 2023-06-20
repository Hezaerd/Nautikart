// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/GreenShell.h"
#include "LevelScript.h"
#include "Kart.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGreenShell::AGreenShell()
{
	PrimaryActorTick.bCanEverTick = true;

	if (!root)
	{
		root = CreateDefaultSubobject<USceneComponent>("Root");
		RootComponent = root;
	}

	if (!sphereCollider)
	{
		sphereCollider = CreateDefaultSubobject<USphereComponent>("SphereCollision");
		sphereCollider->SetupAttachment(root);

		sphereCollider->SetNotifyRigidBodyCollision(true);
		sphereCollider->SetSimulatePhysics(true);
		sphereCollider->SetEnableGravity(false);

		// Query and physics

		sphereCollider->SetCollisionProfileName("BlockAllDynamic");
	}

	if (!mesh)
	{
		mesh = CreateDefaultSubobject<USceneComponent>("Mesh");
		mesh->SetupAttachment(sphereCollider);
	}

	InitialLifeSpan = 15.f;
}

// Called when the game starts or when spawned
void AGreenShell::BeginPlay()
{
	Super::BeginPlay();

	if (sphereCollider)
		sphereCollider->OnComponentHit.AddDynamic(this, &AGreenShell::OnHit);

	ALevelScript* levelScript = Cast<ALevelScript>(GetLevel()->GetLevelScriptActor());
	if (levelScript)
	{
		gravity = levelScript->gravity;
		speed = levelScript->maxSpeed * 2;
	}
}

void AGreenShell::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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

// Called every frame
void AGreenShell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!mesh) return;

	FVector forward = mesh->GetForwardVector();
	FVector right = mesh->GetRightVector();
	FVector down = mesh->GetUpVector() * -1;
	FVector normal;

	FHitResult hit;
	FCollisionQueryParams traceParams;
	traceParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(hit, mesh->GetComponentLocation(), mesh->GetComponentLocation() + down * (sphereCollider->GetScaledSphereRadius() + 0.1f), ECC_Visibility, traceParams))
	{
		normal = hit.ImpactNormal;
		onGround = true;
	}
	else
	{
		normal = FVector(0, 0, 1);
		onGround = false;
	}

	float yaw = mesh->GetComponentRotation().Yaw;
	float pitch = UKismetMathLibrary::MakeRotFromYZ(right, normal).Pitch;
	float roll = UKismetMathLibrary::MakeRotFromXZ(forward, normal).Roll;

	if (GetWorld()->LineTraceSingleByChannel(hit, mesh->GetComponentLocation(), mesh->GetComponentLocation() + forward * (sphereCollider->GetScaledSphereRadius() + 0.1f), ECC_Visibility, traceParams))
	{
		if (Cast<AItem>(hit.GetActor())) return;

		FVector direction = (mesh->GetComponentLocation() + forward) - mesh->GetComponentLocation();
		FVector mirror = UKismetMathLibrary::MirrorVectorByNormal(direction, hit.ImpactNormal);
		yaw = UKismetMathLibrary::MakeRotFromX(mirror).Yaw;
	}

	mesh->SetWorldRotation(FRotator(pitch, yaw, roll));
	forward = mesh->GetForwardVector();

	if (!onGround)
	{
		sphereCollider->AddForce(down * 980.0f, FName("None"), true);
	}
	else
	{
		sphereCollider->SetPhysicsLinearVelocity(FVector::Zero(), false);
	}

	sphereCollider->SetWorldLocation(sphereCollider->GetComponentLocation() + forward * speed * DeltaTime, false, nullptr, ETeleportType::TeleportPhysics);

	mesh->SetWorldLocation(sphereCollider->GetComponentLocation());
}