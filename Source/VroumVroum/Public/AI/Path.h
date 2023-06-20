// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Path.generated.h"

UCLASS()
class VROUMVROUM_API APath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
		USceneComponent* root;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		USplineComponent* path;

	UPROPERTY(EditAnywhere)
		class UBoxComponent* boxCollider;

	UPROPERTY(EditAnywhere)
		APath* nextPath;

	UPROPERTY(EditAnywhere)
		bool bIsNextLoop = false;

	UPROPERTY(EditAnywhere)
		bool bWasPathLoop = false;

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, 
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);


	USplineComponent* GetSplineComponent();
};
