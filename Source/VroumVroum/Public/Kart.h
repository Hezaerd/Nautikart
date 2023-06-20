// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "LevelScript.h"
#include "NiagaraComponent.h"
#include "Components/SplineComponent.h"
#include "AI/Path.h"
#include "Kart.generated.h"

class ACheckPoint;

UCLASS()
class VROUMVROUM_API AKart : public APawn
{
	GENERATED_BODY()

public:
	AKart();
	virtual void Tick(float DeltaTime) override;

	void SetNextCheckPoint(ACheckPoint* overlapActor);

	void AddItem(EItems item);

	FVector GetFrontLocation();
	FVector GetBackLocation();
	FRotator GetRotation();

	void SetCanDrive(bool b);

	UFUNCTION(BlueprintCallable)
		void SetBonusSpeed(float speed);

	UFUNCTION(BlueprintCallable)
		virtual void GetHit();

	UFUNCTION(BlueprintCallable)
		FString GetLapString();

	UFUNCTION(BlueprintCallable)
		UTexture* GetItemImage(int i);

	UFUNCTION(BlueprintCallable)
		FString GetTimer();

	UFUNCTION(BlueprintCallable)
		FVector GetKartLocation();

	UPROPERTY(EditAnywhere, Category = "path")
		APath* activePath;

	UPROPERTY(EditAnywhere, Category = "path")
		APath* loopPath;

	USplineComponent* activeSpline;
	bool	bInLoop = false;

	void SetActivePath(APath* newActivePath);
	void ShouldActivateLoopPath(bool command);


	void ReplaceKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Component")
		USceneComponent* playerRoot;

	UPROPERTY(EditAnywhere, Category = "Component")
		USceneComponent* kart;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* kartMesh;

	UPROPERTY(EditAnywhere)
		USceneComponent* kartHidden;

	UPROPERTY(EditAnywhere)
		USpringArmComponent* springArm;

	UPROPERTY(EditAnywhere, Category = "Component")
		UCameraComponent* camera;

	UPROPERTY(EditAnywhere, Category = "Component")
		USceneComponent* frontLeft;

	UPROPERTY(EditAnywhere, Category = "Component")
		USceneComponent* frontRight;

	UPROPERTY(EditAnywhere, Category = "Component")
		USceneComponent* backLeft;

	UPROPERTY(EditAnywhere, Category = "Component")
		USceneComponent* backRight;

	UPROPERTY(EditAnywhere, Category = "Component")
		USphereComponent* sphereCollision;

	UPROPERTY(EditAnywhere, Category = "Component")
		USceneComponent* frontObjectSpawner;

	UPROPERTY(EditAnywhere, Category = "Component")
		USceneComponent* backObjectSpawner;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* speedParticle;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* hitParticle;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* boostParticle;

	UPROPERTY(EditAnywhere, Category = "sound")
		USoundCue* boostSound;

	UPROPERTY(EditAnywhere, Category = "sound")
		USoundCue* hitSound;

	UPROPERTY(EditAnywhere, Category = "sound")
		USoundCue* pickItemSound;

	UPROPERTY(EditAnywhere, Category = "sound")
		USoundCue* lapSound;

	UPROPERTY(EditAnywhere, Category = "sound")
		USoundCue* finishRaceSound;

	UPROPERTY(EditAnywhere)
		float steerMax = 0.8f;

	UPROPERTY(EditAnywhere)
		float invincibilityFrameTimer = 1.0f;

	UPROPERTY(BlueprintReadOnly)
		float speedRatio;

	float gravity;
	float speedMax;
	float currentSpeed = 0;
	float currentAcceleration = 0;
	float currentSteer = 0;
	float steerInputValue = 0;
	float accelerateInputValue = 0;
	float invincibility = 0;

	float bonusSpeed = 0;
	float playerRollMax = 25.0f;

	bool onGround = false;
	bool canDrive = false;

	TArray<EItems> items;
	bool bisShielded = false;

	int lapCounter = 0;
	ACheckPoint* nextCheckPoint;
	ACheckPoint* previousCheckPoint = nullptr;

	ALevelScript* levelScript;

	FVector GetWheelNormal(USceneComponent* wheel);
	void CheckGround();
	virtual void SetVelocity(float delta);
	virtual void CalcRotationFromNormal(float delta);
	void FollowSphere();

	virtual void Steer(float delta);

	virtual void	UseItem();
};
