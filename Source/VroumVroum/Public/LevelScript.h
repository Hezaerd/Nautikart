// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CheckPoint.h"
#include "Item.h"
#include "Engine/Texture.h"
#include "Engine/LevelScriptActor.h"
#include "LevelScript.generated.h"

/**
 *
 */
UCLASS()
class VROUMVROUM_API ALevelScript : public ALevelScriptActor
{
public:
	GENERATED_BODY()

		ALevelScript();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		ACheckPoint* firstCheckPoint;

	UPROPERTY(EditAnywhere)
		int numOfLaps = 3;

	UPROPERTY(EditAnywhere)
		float gravity = 980.0f;

	UPROPERTY(EditAnywhere)
		float maxSpeed = 1500.0f;

	void UseItem(class AKart* kart, EItems item);

	UTexture* GetItemImage(EItems item);

	FString GetTimer();

private:

	UPROPERTY(EditAnywhere, Category = "Item")
		TSubclassOf<AItem> greenShellBP;

	UPROPERTY(EditAnywhere, Category = "Item")
		TSubclassOf<AItem> bananaBP;

	UPROPERTY(EditAnywhere, Category = "Item")
		TSubclassOf<AItem> bombBP;

	UPROPERTY(EditAnywhere, Category = "Image")
		UTexture* NoItemImage;

	UPROPERTY(EditAnywhere, Category = "Image")
		UTexture* greenShellImage;

	UPROPERTY(EditAnywhere, Category = "Image")
		UTexture* boostImage;

	UPROPERTY(EditAnywhere, Category = "Image")
		UTexture* bananaImage;

	UPROPERTY(EditAnywhere, Category = "Image")
		UTexture* bombImage;

	UPROPERTY(EditAnywhere, Category = "sound")
		USoundCue* countDownSound;

	UPROPERTY(EditAnywhere, Category = "sound")
		USoundCue* startRaceSound;

	float startTimer = 2.42f + 4.99f;
	int soundCounter = 0;
	bool kartCanDrive = false;
};
