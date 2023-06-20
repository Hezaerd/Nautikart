// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kart.h"
#include "Components/AudioComponent.h"
#include "PlayerKart.generated.h"

UCLASS()
class VROUMVROUM_API APlayerKart : public AKart
{
	GENERATED_BODY()

public:
	APlayerKart();
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		USceneComponent* driftForward;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* driftLeftParticle;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* driftRightParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAudioComponent* driftAudioComp;

	bool isDrifting = false;
	float driftMagnitude = 0;
	float driftCancelTimer = 0;

	bool isPaused = false;

	FRotator springArmStartRotator;

	void AccelerateInput(float value);
	void SteerInput(float value);

	void StartDrifting();
	void StopDrifting();

	void LookBehind();
	void Pause();

	FVector Drift(float delta);

	virtual void Steer(float delta) override;
	virtual void SetVelocity(float delta) override;
	virtual void CalcRotationFromNormal(float delta) override;
	virtual void GetHit() override;
};
