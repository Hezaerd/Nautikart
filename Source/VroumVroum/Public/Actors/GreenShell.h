// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Item.h"

#include "GreenShell.generated.h"

UCLASS()
class VROUMVROUM_API AGreenShell : public AItem
{
	GENERATED_BODY()

public:
	AGreenShell();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
		USceneComponent* root;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		USphereComponent* sphereCollider;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		USceneComponent* mesh;

	float speed = 2000.0f;

	bool onGround = false;
	float gravity;
};
