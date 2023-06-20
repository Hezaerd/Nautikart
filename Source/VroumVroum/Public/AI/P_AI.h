// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kart.h"
#include "Actors/PickableRow.h"

#include "P_AI.generated.h"

UCLASS()
class VROUMVROUM_API AP_AI : public AKart
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AP_AI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Displacement")
		UStaticMeshComponent* pathOracle;

	UPROPERTY(EditAnywhere, Category = "Displacement")
		float overrideSpeed = -1;
	
	UPROPERTY(EditAnywhere, Category = "Displacement")
		float baseAccelerateInputValue = 1;

	UPROPERTY(EditAnywhere, Category = "Displacement")
		float timeAfterWrongDirectionToReplace = 2;

	UPROPERTY(EditAnywhere, Category = "Displacement")
		float leastSteerNecessaryToCurveSlow = 0.6;

	UPROPERTY(EditAnywhere, Category = "Displacement")
		float leastSpeedNecessaryToCurveSlow = 1000;

	/* --- AI ---*/
	UPROPERTY(EditAnywhere, Category = "Overtake")
		float overtakeTraceMultiplicator = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Overtake")
		float maxSpeedForMajorBreakPercent = 0.2;

	UPROPERTY(EditAnywhere, Category = "Overtake")
		float OvertakeSpeedPercentReductor = 0.8;

	UPROPERTY(EditAnywhere, Category = "Overtake")
		float overtakeAngle = 40;

	UPROPERTY(EditAnywhere, Category = "Overtake")
		float overtakeAngleMultiplicator = 1.5;

	UPROPERTY(EditAnywhere, Category = "Overtake")
		float forwardTraceAngle = 10;

	UPROPERTY(EditAnywhere, Category = "Overtake")
		FVector sideTracePosition = FVector(-150, 75, 0);

	UPROPERTY(EditAnywhere, Category = "Overtake")
		float sideTraceMultiplicator = 300;

	UPROPERTY(EditAnywhere, Category = "Overtake")
		float sideTraceAngle = 20;

	float	currentAngleMultiplicator = 1;

	//seconds
	UPROPERTY(EditAnywhere, Category = "Pickable")
		float useItemMaxDelay = 15;

	UPROPERTY(EditAnywhere, Category = "Pickable")
		float distanceToDetectPickable = 1500;

	APickable* target = nullptr;

	void	DelayUseItem();

	void ReplaceFromWrongDirection();

protected:
	void AccelerateInput(float value);
	void SteerInput();

	virtual void SetVelocity(float delta) override;
	virtual void Steer(float delta) override;

	float	GetSteerFromPath();
	float	GetSteerFromBehaviour(float currentYaw);
	float	NoSpeedForwardOvertake(float currentYaw);
	float	ForwardOvertake(float currentYAw);
	float	RightForwardOvertake();
	float	LeftForwardOvertake();
	float	SideTraceRight();
	float	SideTraceLeft();
	bool	ForawardLineTrace(FHitResult& hitResult, float traceMultiplicator = 1, bool shouldTakeAngle = false, float angleDirection = 1);
	bool	IsOtherSlower(AP_AI* other);
	void	SetAccelerationToBase();
	void	ReduceAcceleration(float percentToReduceBy);
	void	ReduceSpeedInCurve(float currentAngle);

	TArray<APickableRow*> rows;

	float	ChooseWhoToTakePathFrom();
	float	GetSteerFromClosestPickable(APickableRow* row);

	virtual void UseItem() override;

	bool	ShouldReplace();
	
};
