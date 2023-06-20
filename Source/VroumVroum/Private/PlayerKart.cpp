// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerKart.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/WorldSettings.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayerKart::APlayerKart()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	driftForward = CreateDefaultSubobject<USceneComponent>("DriftForward");
	driftForward->SetupAttachment(playerRoot);

	driftLeftParticle = CreateDefaultSubobject<UNiagaraComponent>("driftLeftParticle");
	driftRightParticle = CreateDefaultSubobject<UNiagaraComponent>("driftRightParticle");

	driftAudioComp = CreateDefaultSubobject<UAudioComponent>("driftAudioComp");

	driftLeftParticle->SetupAttachment(kart);
	driftRightParticle->SetupAttachment(kart);

	driftAudioComp->SetupAttachment(kartHidden);
}

// Called when the game starts or when spawned
void APlayerKart::BeginPlay()
{
	AKart::BeginPlay();

	driftLeftParticle->Deactivate();
	driftRightParticle->Deactivate();

	springArmStartRotator = springArm->GetComponentRotation();
}

// Called every frame
void APlayerKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("accelerate", this, &APlayerKart::AccelerateInput);
	InputComponent->BindAxis("steer", this, &APlayerKart::SteerInput);

	InputComponent->BindAction("drift", IE_Pressed, this, &APlayerKart::StartDrifting);
	InputComponent->BindAction("drift", IE_Released, this, &APlayerKart::StopDrifting);

	InputComponent->BindAction("invertCamera", IE_Pressed, this, &APlayerKart::LookBehind);
	InputComponent->BindAction("invertCamera", IE_Released, this, &APlayerKart::LookBehind);

	InputComponent->BindAction("Pause", IE_Pressed, this, &APlayerKart::Pause);

	InputComponent->BindAction("throw", IE_Pressed, this, &APlayerKart::UseItem);
}

void APlayerKart::AccelerateInput(float value)
{
	accelerateInputValue = value;
}

void APlayerKart::SteerInput(float value)
{
	steerInputValue = value;
}

void APlayerKart::Pause()
{
	AWorldSettings* world = GetWorldSettings();
	if (world)
	{
		if (isPaused)
		{
			GetWorldSettings()->SetTimeDilation(1);
		}
		else
		{
			GetWorldSettings()->SetTimeDilation(0);
		}
		isPaused = !isPaused;
	}
}

void APlayerKart::StartDrifting()
{
	//must be faster than 60% of his max speed to start drifting
	if (currentSpeed >= speedMax * 0.6 && onGround)
	{
		driftAudioComp->Play(0);
		isDrifting = true;
		//driftForward->SetWorldRotation(kartHidden->GetComponentRotation());
		driftMagnitude = 0;
		driftCancelTimer = 0;
	}
}

void APlayerKart::StopDrifting()
{
	if (!isDrifting) return;

	driftAudioComp->Stop();
	isDrifting = false;
	driftLeftParticle->Deactivate();
	driftRightParticle->Deactivate();

	//bonus to discuss
	if (driftMagnitude > 0.25)
	{
		bonusSpeed = 0.05f;
	}
}

void APlayerKart::LookBehind()
{
	springArmStartRotator = springArmStartRotator.Add(0, 180, 0);
	springArm->SetRelativeRotation(springArmStartRotator);
}

FVector APlayerKart::Drift(float delta)
{
	if (kartMesh->GetRelativeRotation().Pitch < 0)
	{
		driftLeftParticle->Activate();
		driftRightParticle->Deactivate();
	}
	else
	{
		driftRightParticle->Activate();
		driftLeftParticle->Deactivate();
	}

	FVector forward = (kartHidden->GetForwardVector() + driftForward->GetForwardVector()) / 2;

	driftMagnitude += delta * (1 - forward.Length());

	//wait after anim to check, magnitude to discuss
	if (forward.Length() > 0.98f)
	{
		driftCancelTimer += delta;
		if (driftCancelTimer > 0.5f)
			StopDrifting();
	}
	else
	{
		driftCancelTimer = 0;
	}

	FRotator lerp = FMath::Lerp(driftForward->GetComponentRotation(), kartHidden->GetComponentRotation(), delta / 1.75f);
	driftForward->SetWorldRotation(lerp);

	forward.Normalize();

	return forward;
}

void APlayerKart::Steer(float delta)
{
	if (isDrifting)
	{
		currentSteer = FMath::Lerp(currentSteer, steerInputValue, delta * 6);
	}
	else
	{
		currentSteer = FMath::Lerp(currentSteer, steerInputValue, delta * 3);
	}

	AKart::Steer(delta);
}

void APlayerKart::SetVelocity(float delta)
{
	DrawDebugLine(
		GetWorld(),
		kartHidden->GetComponentLocation(),
		kartHidden->GetComponentLocation() + kartHidden->GetForwardVector() * 1000,
		FColor::Blue,
		false,
		-1, 0, 2
	);

	DrawDebugLine(
		GetWorld(),
		kartHidden->GetComponentLocation(),
		kartHidden->GetComponentLocation() + driftForward->GetForwardVector() * 1000,
		FColor::Blue,
		false,
		-1, 0, 2
	);

	FVector forward = (kartHidden->GetForwardVector() + driftForward->GetForwardVector()) / 2;

	if (isDrifting)
	{
		forward = Drift(delta);
	}
	else
	{
		FRotator lerp = FMath::Lerp(driftForward->GetComponentRotation(), kartHidden->GetComponentRotation(), delta * 6);
		driftForward->SetWorldRotation(lerp);
	}

	DrawDebugLine(
		GetWorld(),
		kartHidden->GetComponentLocation(),
		kartHidden->GetComponentLocation() + forward * 1000,
		FColor::Red,
		false,
		-1, 0, 2
	);

	if (!onGround)
	{
		FVector downVec = kartHidden->GetUpVector() * -1;
		sphereCollision->AddForce(downVec * gravity, FName("None"), true);
	}
	else
	{
		sphereCollision->SetPhysicsLinearVelocity(FVector::Zero(), false);
	}

	sphereCollision->SetWorldLocation(sphereCollision->GetComponentLocation() + forward * currentSpeed * delta, false, nullptr, ETeleportType::TeleportPhysics);
}

void APlayerKart::CalcRotationFromNormal(float delta)
{
	AKart::CalcRotationFromNormal(delta);

	if (isDrifting)
	{
		float driftYaw = driftForward->GetComponentRotation().Yaw;
		driftForward->SetWorldRotation(FRotator(kartHidden->GetComponentRotation().Pitch, driftYaw, kartHidden->GetComponentRotation().Roll));
	}
}

void APlayerKart::GetHit()
{
	AKart::GetHit();

	StopDrifting();
}