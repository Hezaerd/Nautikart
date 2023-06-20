// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/P_AI.h"
#include <Kismet/KismetMathLibrary.h>
#include "Engine/World.h"
#include <Kismet/GameplayStatics.h>
#include "DrawDebugHelpers.h"


// Sets default values
AP_AI::AP_AI()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pathOracle = CreateDefaultSubobject<UStaticMeshComponent>("oracle");
	pathOracle->SetupAttachment(kartHidden);
}

// Called when the game starts or when spawned
void AP_AI::BeginPlay()
{
	Super::BeginPlay();

	/* --- AI --- */
	if (overrideSpeed != -1)
	{
		speedMax = overrideSpeed;
	}
	AccelerateInput(baseAccelerateInputValue);

	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickableRow::StaticClass(), foundActors);

	for (AActor* actor : foundActors)
	{
		APickableRow* row = Cast<APickableRow>(actor);
		rows.Add(row);
	}
}

// Called every frame
void AP_AI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ReplaceFromWrongDirection();
}

void AP_AI::AccelerateInput(float value)
{
	accelerateInputValue = value;
}

void AP_AI::SteerInput()
{
	float value = 0;

	value += ChooseWhoToTakePathFrom();

	if (!bInLoop && onGround)
		value += GetSteerFromBehaviour(value);

	float newSteer = (float)UKismetMathLibrary::MapRangeClamped(value, -90, 90, -1, 1);

	steerInputValue = newSteer;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::SanitizeFloat(steerInputValue));
}

void AP_AI::SetVelocity(float delta)
{
	if (!onGround)
	{
		FVector downVec = kartHidden->GetUpVector() * -1;
		sphereCollision->AddForce(downVec * gravity, FName("None"), true);
	}
	else
	{
		sphereCollision->SetPhysicsLinearVelocity(FVector::Zero(), false);
	}

	FVector forward = kartHidden->GetForwardVector();
	sphereCollision->SetWorldLocation(sphereCollision->GetComponentLocation() + forward * currentSpeed * delta,
		false, nullptr, ETeleportType::TeleportPhysics);

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::SanitizeFloat(currentSpeed));

}

void AP_AI::Steer(float delta)
{
	SteerInput();

	float newZ = delta * steerInputValue;
	kart->AddLocalRotation(FQuat(0, 0, newZ, 1));
	kartHidden->AddLocalRotation(FQuat(0, 0, newZ, 1));

	steerInputValue = 0;
}



float AP_AI::GetSteerFromPath()
{
	if (activeSpline == nullptr)
		return 0;

	FVector worldLocation;

	if (bInLoop)
	{
		worldLocation = kartHidden->GetComponentLocation();
	}
	else
		worldLocation = pathOracle->GetComponentLocation();

	//FVector worldLocation = GetActorLocation(); //root doesn't move
	FVector closestTangent =
		activeSpline->FindTangentClosestToWorldLocation(worldLocation, ESplineCoordinateSpace::World);

	closestTangent.Normalize();
	closestTangent *= 300; //target further point on spline

	FVector targetPoint = worldLocation + closestTangent;

	FVector	closestLocationToTarget =
		activeSpline->FindLocationClosestToWorldLocation(targetPoint, ESplineCoordinateSpace::World);

	FRotator lookAt = UKismetMathLibrary::FindLookAtRotation(worldLocation, closestLocationToTarget);
	//FRotator actorRot = GetActorRotation();
	FRotator actorRot = kartHidden->GetComponentRotation();

	FRotator eulerAngle = lookAt - actorRot;
	eulerAngle.Normalize();

	if (bInLoop)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::SanitizeFloat(eulerAngle.Pitch));

		ReduceAcceleration(0.5);

		float dotUpUp = FVector::DotProduct(kartHidden->GetUpVector(), GetActorUpVector());
		float dotForwardUp = FVector::DotProduct(kartHidden->GetForwardVector(), GetActorUpVector());

		if (abs(dotUpUp) < abs(dotForwardUp))
		{
			if (dotUpUp < 0)
				return eulerAngle.Yaw * -1;
		}
		else
		{
			if (dotForwardUp > 0)
				return eulerAngle.Pitch * 1;
			else
				return eulerAngle.Pitch * -1;
		}
	}

	ReduceSpeedInCurve(eulerAngle.Yaw);
	return eulerAngle.Yaw;
}

float AP_AI::GetSteerFromBehaviour(float currentYaw)
{
	float steerAmount = 0;

	steerAmount += NoSpeedForwardOvertake(currentYaw);
	steerAmount += ForwardOvertake(steerAmount + currentYaw);
	steerAmount += RightForwardOvertake();
	steerAmount += LeftForwardOvertake();
	steerAmount += SideTraceRight();
	steerAmount += SideTraceLeft();

	return steerAmount;
}

float AP_AI::NoSpeedForwardOvertake(float currentYaw)
{
	FHitResult rayResult;
	bool hasHit = ForawardLineTrace(rayResult, 1.3);
	
	if (!hasHit)
		return 0;

	AP_AI* other = Cast<AP_AI>(rayResult.GetActor());
	if (other == nullptr)
		return 0;

	USceneComponent* otherKart = other->kartHidden;

	if (this->currentSpeed < this->speedMax * this->maxSpeedForMajorBreakPercent ||
		other->currentSpeed > other->speedMax * other->maxSpeedForMajorBreakPercent)
	{
		currentAngleMultiplicator = 1;

		return 0;
	}

	currentAngleMultiplicator = overtakeAngleMultiplicator;

	ReduceAcceleration(OvertakeSpeedPercentReductor);

	float yawAmount = overtakeAngle * currentAngleMultiplicator;

	if (currentYaw >= 0)
		yawAmount *= -1;

	return yawAmount;
}

float AP_AI::ForwardOvertake(float currentYaw)
{
	FHitResult rayResult;
	bool hasHit = ForawardLineTrace(rayResult);

	if (!hasHit)
		return 0;

	AP_AI* other = Cast<AP_AI>(rayResult.GetActor());
	if (other == nullptr)
		return 0;

	if (!IsOtherSlower(other))
		return 0;

	if (OvertakeSpeedPercentReductor * baseAccelerateInputValue < accelerateInputValue)
		ReduceAcceleration(OvertakeSpeedPercentReductor);

	float yawAmount = overtakeAngle * currentAngleMultiplicator;

	if (currentYaw >= 0)
		yawAmount *= -1;

	return yawAmount;
}

float AP_AI::RightForwardOvertake()
{
	FHitResult rayResult;
	bool hasHit = ForawardLineTrace(rayResult, 1, true);

	if (!hasHit)
		return 0;

	AP_AI* other = Cast<AP_AI>(rayResult.GetActor());
	if (other == nullptr)
		return 0;

	if (!IsOtherSlower(other))
		return 0;
	
	if (OvertakeSpeedPercentReductor * baseAccelerateInputValue < accelerateInputValue)
		ReduceAcceleration(OvertakeSpeedPercentReductor);

	float yawAmount = overtakeAngle * overtakeAngleMultiplicator * -1;

	return yawAmount;
}

float AP_AI::LeftForwardOvertake()
{
	FHitResult rayResult;
	bool hasHit = ForawardLineTrace(rayResult, 1, true, -1);

	if (!hasHit)
		return 0;

	AP_AI* other = Cast<AP_AI>(rayResult.GetActor());
	if (other == nullptr)
		return 0;

	if (!IsOtherSlower(other))
		return 0;

	if (OvertakeSpeedPercentReductor * baseAccelerateInputValue < accelerateInputValue)
		ReduceAcceleration(OvertakeSpeedPercentReductor);

	float yawAmount = overtakeAngle * overtakeAngleMultiplicator;

	return yawAmount;
}

float AP_AI::SideTraceRight()
{
	FVector worldLocation = kartHidden->GetComponentLocation();
	FVector up = kartHidden->GetUpVector();
	FVector forward = kartHidden->GetForwardVector();
	FVector right = kartHidden->GetRightVector();

	FVector startX = right * sideTracePosition.Y;
	FVector startY = forward * sideTracePosition.X;
	FVector startZ = up * sideTracePosition.Z;

	FVector startPos = worldLocation + startX + startY + startZ;
	FVector endPos = startPos + forward * sideTraceMultiplicator;

	FHitResult hitResult;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	bool hasHit =
		GetWorld()->LineTraceSingleByChannel(hitResult, startPos, endPos, ECollisionChannel::ECC_Visibility, QueryParams);
	DrawDebugLine(GetWorld(), startPos, endPos, hitResult.bBlockingHit ? FColor::Green : FColor::Red, false, -1, 0, 2.0f);

	if (!hasHit)
		return 0;

	AP_AI* other = Cast<AP_AI>(hitResult.GetActor());
	if (other == nullptr)
		return 0;

	return sideTraceAngle * -1;
}

float AP_AI::SideTraceLeft()
{
	FVector worldLocation = kartHidden->GetComponentLocation();
	FVector up = kartHidden->GetUpVector();
	FVector forward = kartHidden->GetForwardVector();
	FVector right = kartHidden->GetRightVector();

	FVector startX = right * -sideTracePosition.Y;
	FVector startY = forward * sideTracePosition.X;
	FVector startZ = up * sideTracePosition.Z;

	FVector startPos = worldLocation + startX + startY + startZ;
	FVector endPos = startPos + forward * sideTraceMultiplicator;

	FHitResult hitResult;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	bool hasHit =
		GetWorld()->LineTraceSingleByChannel(hitResult, startPos, endPos, ECollisionChannel::ECC_Visibility, QueryParams);
	DrawDebugLine(GetWorld(), startPos, endPos, hitResult.bBlockingHit ? FColor::Green : FColor::Red, false, -1, 0, 2.0f);

	if (!hasHit)
		return 0;

	AP_AI* other = Cast<AP_AI>(hitResult.GetActor());
	if (other == nullptr)
		return 0;

	return sideTraceAngle;
}

bool AP_AI::ForawardLineTrace(FHitResult& hitResult, float traceMultiplicator, bool shouldTakeAngle, float angleDirection)
{
	FVector worldLocation	= kartHidden->GetComponentLocation();
	FVector up				= kartHidden->GetUpVector();
	FVector forward			= kartHidden->GetForwardVector();

	FVector startPos	= worldLocation + up * 1;
	FVector endPos		= startPos + forward * (overtakeTraceMultiplicator * traceMultiplicator);

	if (shouldTakeAngle)
	{
		FVector angleAxis = kartHidden->GetForwardVector().RotateAngleAxis(forwardTraceAngle * angleDirection, kartHidden->GetUpVector());

		endPos = startPos + angleAxis * overtakeTraceMultiplicator;
	}

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	bool hasHit =
		GetWorld()->LineTraceSingleByChannel(hitResult, startPos, endPos, ECollisionChannel::ECC_Visibility, QueryParams);
	DrawDebugLine(GetWorld(), startPos, endPos, hitResult.bBlockingHit ? FColor::Green : FColor::Red, false, -1, 0, 2.0f);

	return hasHit;
}

void AP_AI::ReduceAcceleration(float percentToReduceBy)
{
	accelerateInputValue = baseAccelerateInputValue * percentToReduceBy;
}

void AP_AI::ReduceSpeedInCurve(float currentAngle)
{
	float steer = (float)UKismetMathLibrary::MapRangeClamped(currentAngle, -90, 90, -1, 1);
	float absSteer = UKismetMathLibrary::Abs(steer);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::SanitizeFloat(currentSpeed));

	if (absSteer >= leastSteerNecessaryToCurveSlow)
	{
		if (currentSpeed >= leastSpeedNecessaryToCurveSlow)
		{
			ReduceAcceleration(0.5);
			return;
		}

		ReduceAcceleration(0.7);
		return;
	}

	SetAccelerationToBase();
}

float AP_AI::ChooseWhoToTakePathFrom()
{
	APickableRow* closestRow = nullptr;

	for (APickableRow* row : rows)
	{
		float dist = FVector::Dist(kartHidden->GetComponentLocation(), row->GetActorLocation());

		FVector direction = row->GetActorLocation() - kartHidden->GetComponentLocation();
		direction.Normalize();
		float dot = FVector::DotProduct(kartHidden->GetForwardVector(), direction);

		if (dist <= distanceToDetectPickable)
		{
			if (dot > 0)
			{
				closestRow = row;
				break;
			}
			else
			{
				target = nullptr;
			}
			
		}
	}

	if (closestRow != nullptr)
	{
		return GetSteerFromClosestPickable(closestRow);
	}

	return GetSteerFromPath();
}

float AP_AI::GetSteerFromClosestPickable(APickableRow* row)
{
	FVector worldLocation = kartHidden->GetComponentLocation();
	FVector closestPickable;

	if (target != nullptr)
	{
		closestPickable = target->GetActorLocation();
	}
	else
	{
		TArray<FVector> position;
		for (APickable* pickable : row->row)
		{
			if (!pickable->bIsEnable)
				continue;

			position.Add(pickable->GetActorLocation());
		}

		closestPickable = position[0];

		for (FVector pos : position)
		{
			float distance = FVector::Dist(worldLocation, closestPickable);
			float distance1 = FVector::Dist(worldLocation, pos);

			if (distance1 <= distance)
				closestPickable = pos;
		}
	}

	FRotator lookAt = UKismetMathLibrary::FindLookAtRotation(worldLocation, closestPickable);
	FRotator actorRot = kartHidden->GetComponentRotation();

	FRotator eulerAngle = lookAt - actorRot;
	eulerAngle.Normalize();

	return eulerAngle.Yaw;
}

void AP_AI::DelayUseItem()
{
	if (items[0] == EItems::NoItem)
		return;

	double delay = FMath::RandRange(0.f, useItemMaxDelay);

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]
		{
			UseItem();
		});

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, delay, false);
}

void AP_AI::UseItem()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::SanitizeFloat(items[0]));
	AKart::UseItem();

	if (items[0] != EItems::NoItem)
		DelayUseItem();
}

void AP_AI::ReplaceFromWrongDirection()
{
	if (ShouldReplace())
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([&]
			{
				if (ShouldReplace())
					AKart::ReplaceKart();
			});

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 
			timeAfterWrongDirectionToReplace, false);
	}
}

bool AP_AI::ShouldReplace()
{
	if (bInLoop && !onGround)
		return false;

	FVector worldLocation = kartHidden->GetComponentLocation();
	float inputKey = activeSpline->FindInputKeyClosestToWorldLocation(worldLocation);
	float nextImputKey = inputKey + 1;

	FVector direction = activeSpline->GetDirectionAtSplineInputKey(inputKey, ESplineCoordinateSpace::World);
	FVector	forward = kartHidden->GetForwardVector();

	float	dot = FVector::DotProduct(direction, forward);

	//dot inferior means not the same direction
	if (dot < 0)
		return true;
	
	return false;
}

bool AP_AI::IsOtherSlower(AP_AI* other)
{
	if (other->currentSpeed < this->currentSpeed)
		return true;

	return false;
}

void AP_AI::SetAccelerationToBase()
{
	accelerateInputValue = baseAccelerateInputValue;
}