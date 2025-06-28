// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeamPlayerStart.generated.h"

UCLASS()
class DUOSHOOTING_API ATeamPlayerStart : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATeamPlayerStart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
