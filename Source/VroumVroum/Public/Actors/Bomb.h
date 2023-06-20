// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"

#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Bomb.generated.h"

/**
 *
 */
UCLASS()
class VROUMVROUM_API ABomb : public AItem
{
	GENERATED_BODY()

public:
	ABomb();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void Explode();
	void DestroyBomb();

private:

	// Components
	UPROPERTY(VisibleAnywhere, Category = "Components")
		USceneComponent* root;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		USphereComponent* sphereCollider;

	UPROPERTY(EditAnywhere, Category = "Components")
		UStaticMeshComponent* mesh;

	// Explosion
	float explosionRadius = 10.f;
	float explosionTime = 0.25f;

	float explosionRatio = 0;

	// Timer
	float timeToExplode = 3.f;
	FTimerHandle timerHandle;
	bool bExplode = false;
	FVector startScale;
};
