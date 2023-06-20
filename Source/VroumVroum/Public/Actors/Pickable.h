// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"

#include "TimerManager.h"
#include "../Item.h"

#include "Pickable.generated.h"

UCLASS()
class VROUMVROUM_API APickable : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	APickable();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	EItems GenerateRandomItem();

	void Disable();
	void Enable();
	bool bIsEnable = true;

private:
	UPROPERTY(EditAnywhere, Category = "Components")
		class UBoxComponent* BoxCollider;

	UPROPERTY(EditAnywhere, Category = "Components")
		class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "sound")
		USoundCue* pickUpSound;

	float respawnTime = 5.f;
	FTimerHandle respawnTimer;
};
