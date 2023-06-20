// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "Item.generated.h"

UENUM()
enum EItems
{
	NoItem,
	GreenShell,
	Boost,
	Banana,
	Bomb
};

UCLASS()
class VROUMVROUM_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "sound")
		USoundCue* destroySound;
};
