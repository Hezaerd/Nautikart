// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelScript.h"
#include "Kart.h"

#include "Kismet/GameplayStatics.h"

ALevelScript::ALevelScript()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ALevelScript::BeginPlay()
{
	Super::BeginPlay();
}

void ALevelScript::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (startTimer < 0) return;

	if (startTimer <= 2.0f && !kartCanDrive)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AKart::StaticClass(), FoundActors);

		for (AActor* actor : FoundActors)
		{
			AKart* kart = Cast<AKart>(actor);
			if (kart)
				kart->SetCanDrive(true);
		}
		kartCanDrive = true;
		UGameplayStatics::PlaySound2D(this, startRaceSound);
	}

	startTimer -= DeltaTime;
}

void ALevelScript::UseItem(AKart* kart, EItems item)
{
	FActorSpawnParameters params;

	switch (item)
	{
	case EItems::GreenShell:
		GetWorld()->SpawnActor<AItem>(greenShellBP, kart->GetFrontLocation(), kart->GetRotation(), params);
		break;
	case EItems::Boost:
		kart->SetBonusSpeed(0.15f);
		break;
	case EItems::Banana:
		GetWorld()->SpawnActor<AItem>(bananaBP, kart->GetBackLocation(), kart->GetRotation(), params);
		break;
	case EItems::Bomb:
		GetWorld()->SpawnActor<AItem>(bombBP, kart->GetBackLocation(), kart->GetRotation(), params);
		break;
	default:
		return;
	}
}

UTexture* ALevelScript::GetItemImage(EItems item)
{
	switch (item)
	{
	case EItems::GreenShell:
		return greenShellImage;
		break;
	case EItems::Boost:
		return boostImage;
		break;
	case EItems::Banana:
		return bananaImage;
		break;
	case EItems::Bomb:
		return bombImage;
		break;
	case EItems::NoItem:
		return NoItemImage;
		break;
	default:
		return NoItemImage;
		break;
	}

	return nullptr;
}

FString ALevelScript::GetTimer()
{
	int n = FMath::Floor(startTimer - 1.0f);

	if (n < 0 || n > 3) return FString("");

	if (n == 0) return FString("Start!");

	if (n != soundCounter)
	{
		UGameplayStatics::PlaySound2D(this, countDownSound);
		soundCounter = n;
	}

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::FromInt(n));
	return FString::FromInt(n);
}