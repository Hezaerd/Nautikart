// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Pickable.h"

#include "PickableRow.generated.h"

UCLASS()
class VROUMVROUM_API APickableRow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickableRow();

	UPROPERTY(BlueprintReadWrite)
		TArray<APickable*> row;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
