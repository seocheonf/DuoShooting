// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PickPhasePawn.generated.h"

UCLASS()
class DUOSHOOTING_API APickPhasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APickPhasePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	//==변수==//
private:
	TSubclassOf<class UPickPhaseUI> OriginPickPhaseUI;
	class UPickPhaseUI* PickPhaseUI;
	//==함수==//
private:
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetPlayerHero(EHeroInfo playerHero);
	UFUNCTION(Server, Reliable)
	void ServerRPC_RespawnPlayer();
public:
};
