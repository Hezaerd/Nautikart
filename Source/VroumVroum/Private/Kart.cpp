// Fill out your copyright notice in the Description page of Project Settings.

#include "Kart.h"
#include "Containers/UnrealString.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "..\Public\Kart.h"

// Sets default values
AKart::AKart()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	playerRoot = CreateDefaultSubobject<USceneComponent>("PlayerRoot");
	RootComponent = playerRoot;

	kart = CreateDefaultSubobject<USceneComponent>("Kart");
	kart->SetupAttachment(playerRoot);

	kartHidden = CreateDefaultSubobject<USceneComponent>("KartHidden");
	kartHidden->SetupAttachment(playerRoot);

	sphereCollision = CreateDefaultSubobject<USphereComponent>("SphereCollision");
	sphereCollision->SetupAttachment(playerRoot);

	kartMesh = CreateDefaultSubobject<UStaticMeshComponent>("KartMesh");
	kartMesh->SetupAttachment(kart);

	springArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	springArm->SetupAttachment(kart);
	camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	camera->SetupAttachment(springArm);

	frontLeft = CreateDefaultSubobject<USceneComponent>("FrontLeft");
	frontLeft->SetupAttachment(kartHidden);

	frontRight = CreateDefaultSubobject<USceneComponent>("FrontRight");
	frontRight->SetupAttachment(kartHidden);

	backLeft = CreateDefaultSubobject<USceneComponent>("BackLeft");
	backLeft->SetupAttachment(kartHidden);

	backRight = CreateDefaultSubobject<USceneComponent>("BackRight");
	backRight->SetupAttachment(kartHidden);

	speedParticle = CreateDefaultSubobject<UNiagaraComponent>("speedParticle");
	speedParticle->SetupAttachment(kart);
	hitParticle = CreateDefaultSubobject<UNiagaraComponent>("hitParticle");
	hitParticle->SetupAttachment(kart);
	boostParticle = CreateDefaultSubobject<UNiagaraComponent>("boostParticle");
	boostParticle->SetupAttachment(kart);
	frontObjectSpawner = CreateDefaultSubobject<USceneComponent>("FrontObjectSpawner");
	frontObjectSpawner->SetupAttachment(kartHidden);

	backObjectSpawner = CreateDefaultSubobject<USceneComponent>("BackObjectSpawner");
	backObjectSpawner->SetupAttachment(kartHidden);
}

// Called when the game starts or when spawned

void AKart::BeginPlay()
{
	Super::BeginPlay();

	levelScript = Cast<ALevelScript>(GetLevel()->GetLevelScriptActor());

	if (levelScript)
	{
		nextCheckPoint = levelScript->firstCheckPoint;
		gravity = levelScript->gravity;
		speedMax = levelScript->maxSpeed;
	}

	sphereCollision->SetWorldLocation(playerRoot->GetComponentLocation());
	kart->SetWorldLocation(playerRoot->GetComponentLocation());
	kartHidden->SetWorldLocation(playerRoot->GetComponentLocation());

	speedParticle->Deactivate();
	hitParticle->Deactivate();
	boostParticle->Deactivate();

	items.Add(EItems::NoItem);
	items.Add(EItems::NoItem);

	SetActivePath(activePath);
}

// Called every frame

void AKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!canDrive) return;

	if (invincibility > 0)
		invincibility -= DeltaTime;

	currentAcceleration = FMath::Lerp(currentAcceleration, accelerateInputValue, DeltaTime);
	currentSpeed = FMath::Lerp(currentSpeed, (currentAcceleration * speedMax), DeltaTime * 3);

	if (bonusSpeed > 0)
	{
		currentSpeed += bonusSpeed * speedMax;
	}

	speedRatio = currentSpeed / speedMax;

	bonusSpeed = FMath::Lerp(bonusSpeed, 0, DeltaTime);

	if (bonusSpeed < 0.02f)
	{
		boostParticle->Deactivate();
		speedParticle->Deactivate();
	}
	else
	{
		boostParticle->Activate();
		speedParticle->Activate();
	}

	CalcRotationFromNormal(DeltaTime);

	CheckGround();

	Steer(DeltaTime);

	SetVelocity(DeltaTime);

	FollowSphere();
}

void AKart::SetNextCheckPoint(ACheckPoint* overlapActor)
{
	if (overlapActor == nextCheckPoint)
	{	
		previousCheckPoint = overlapActor;
		nextCheckPoint = overlapActor->nextCheckPoint;

		if (levelScript)
		{
			if (levelScript->firstCheckPoint == overlapActor)
			{
				lapCounter++;

				if (lapCounter > levelScript->numOfLaps)
				{
					UGameplayStatics::PlaySound2D(this, finishRaceSound);
					canDrive = false;
					return;
				}
				UGameplayStatics::PlaySound2D(this, lapSound);
			}
		}
	}
}

void AKart::AddItem(EItems item)
{
	if (items[0] == EItems::NoItem)
	{
		items[0] = item;
		UGameplayStatics::PlaySound2D(this, pickItemSound);
	}
	else if (items[1] == EItems::NoItem)
	{
		items[1] = item;
		UGameplayStatics::PlaySound2D(this, pickItemSound);
	}
}

void AKart::CalcRotationFromNormal(float delta)
{
	FVector result = GetWheelNormal(frontRight);

	result += GetWheelNormal(frontLeft);
	result += GetWheelNormal(backRight);
	result += GetWheelNormal(backLeft);
	result /= 4;

	FVector up = kartHidden->GetUpVector();

	FVector RotationAxis = FVector::CrossProduct(up, result);
	RotationAxis.Normalize();

	float DotProduct = FVector::DotProduct(up, result);
	float RotationAngle = acosf(DotProduct);

	FQuat Quat = FQuat(RotationAxis, RotationAngle);
	FQuat RootQuat = kartHidden->GetComponentQuat();

	FQuat NewQuat = Quat * RootQuat;

	kartHidden->SetWorldRotation(NewQuat.Rotator());

	FRotator lerpRotation = FMath::Lerp(kart->GetComponentRotation(), kartHidden->GetComponentRotation(), delta * 5);

	kart->SetWorldRotation(lerpRotation);
}

FVector AKart::GetWheelNormal(USceneComponent* wheel)
{
	FVector downVec = wheel->GetUpVector() * -1;

	FHitResult hit;

	FCollisionQueryParams traceParams;

	traceParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(hit, wheel->GetComponentLocation(), wheel->GetComponentLocation() + downVec * 100.0f, ECC_Visibility, traceParams))
		return hit.ImpactNormal;

	return FVector(0, 0, 1);
}

void AKart::CheckGround()
{
	FVector downVec = kartHidden->GetUpVector() * -1;

	FHitResult hit;

	FCollisionQueryParams traceParams;

	traceParams.AddIgnoredActor(this);

	//dist is sphere radius

	if (GetWorld()->LineTraceSingleByChannel(hit, kartHidden->GetComponentLocation(), kartHidden->GetComponentLocation() + downVec * (sphereCollision->GetScaledSphereRadius() + 0.1f), ECC_Visibility, traceParams))
	{
		onGround = true;
	}

	else
	{
		onGround = false;
	}
}

void AKart::FollowSphere()
{
	kart->SetWorldLocation(sphereCollision->GetComponentLocation());

	kartHidden->SetWorldLocation(sphereCollision->GetComponentLocation());
}

void AKart::Steer(float delta)
{
	float newZ = delta * currentSteer * steerMax;

	kart->AddLocalRotation(FQuat(0, 0, newZ, 1));

	kartHidden->AddLocalRotation(FQuat(0, 0, newZ, 1));

	float lerp = FMath::Lerp(kartMesh->GetRelativeRotation().Pitch, currentSteer * playerRollMax, delta * 2);

	FRotator newRot = kartMesh->GetRelativeRotation();

	newRot.Pitch = lerp;

	kartMesh->SetRelativeRotation(newRot);
}

void AKart::UseItem()
{
	if (items[0] == EItems::NoItem)
	{
		return;
	}

	if (levelScript)
		levelScript->UseItem(this, items[0]);

	items[0] = items[1];
	items[1] = EItems::NoItem;
}

FVector AKart::GetFrontLocation()
{
	return frontObjectSpawner->GetComponentLocation();
}

FVector AKart::GetBackLocation()
{
	return backObjectSpawner->GetComponentLocation();
}

FRotator AKart::GetRotation()
{
	return kartHidden->GetComponentRotation();
}

void AKart::SetBonusSpeed(float speed)
{
	bonusSpeed = speed;
	UGameplayStatics::PlaySoundAtLocation(this, boostSound, kartHidden->GetComponentLocation(), kartHidden->GetComponentRotation());
}

void AKart::GetHit()
{
	if (invincibility > 0) return;

	UGameplayStatics::PlaySoundAtLocation(this, hitSound, kartHidden->GetComponentLocation(), kartHidden->GetComponentRotation());

	currentSpeed = 0;

	currentAcceleration = 0;

	currentSteer = 0;

	bonusSpeed = 0;

	invincibility = invincibilityFrameTimer;

	hitParticle->Activate(true);
}

void AKart::SetVelocity(float delta)
{
}

FString AKart::GetLapString()
{
	int laps = 1;
	int current = 1;

	if (lapCounter > 1)
	{
		current = lapCounter;
	}

	if (levelScript)
		laps = levelScript->numOfLaps;

	if (current > laps) 
	{
		current = laps;
	}

	return FString::FromInt(current) + "/" + FString::FromInt(laps);
}

UTexture* AKart::GetItemImage(int i)
{
	if (i > items.Num() - 1) return nullptr;

	if (levelScript)
		return levelScript->GetItemImage(items[i]);

	return nullptr;
}

FString AKart::GetTimer()
{
	if (levelScript)
		return levelScript->GetTimer();

	return FString("");
}

FVector AKart::GetKartLocation()
{
	return kartHidden->GetComponentLocation();
}

void AKart::SetCanDrive(bool b)
{
	canDrive = b;
}

void AKart::SetActivePath(APath* newActivePath)
{
	if (newActivePath == nullptr)
		return;

	activePath = newActivePath;
	activeSpline = newActivePath->GetSplineComponent();
}

void AKart::ShouldActivateLoopPath(bool command)
{
	if (loopPath == nullptr)
		return;

	bInLoop = command;

	if (bInLoop)
		activeSpline = loopPath->GetSplineComponent();
}

void AKart::ReplaceKart()
{

	FVector newPos = this->GetActorLocation();
	FRotator newRot = this->GetActorRotation();

	if (previousCheckPoint != nullptr)
	{
		newPos = previousCheckPoint->GetActorLocation();
		newRot = previousCheckPoint->GetActorRotation();
	}
	
	sphereCollision->SetWorldLocationAndRotation(newPos, newRot, false, nullptr,
		ETeleportType::ResetPhysics);
	currentAcceleration = 0;
	currentSpeed = 0;
	bonusSpeed = 0;
}