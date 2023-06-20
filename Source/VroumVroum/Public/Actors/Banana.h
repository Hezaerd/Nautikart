// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"

#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Banana.generated.h"

/**
 *
 */
UCLASS()
class VROUMVROUM_API ABanana : public AItem
{
	GENERATED_BODY()

public:
	ABanana();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = "Components")
		USceneComponent* root;

	UPROPERTY(EditAnywhere, Category = "Components")
		UCapsuleComponent* capsuleCollider;

	UPROPERTY(EditAnywhere, Category = "Components")
		USceneComponent* mesh;

	void CurvedSpawn();
};
