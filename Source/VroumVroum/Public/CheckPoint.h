// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "CheckPoint.generated.h"

UCLASS()
class VROUMVROUM_API ACheckPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckPoint();

	UPROPERTY(EditAnywhere)
		ACheckPoint* nextCheckPoint;

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected :
	virtual void BeginPlay() override;

private :
	UPROPERTY(EditAnywhere)
		UBoxComponent* boxCollision;
};
