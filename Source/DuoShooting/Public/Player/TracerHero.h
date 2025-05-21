// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HeroBase.h"
#include "TracerHero.generated.h"

UCLASS()
class DUOSHOOTING_API ATracerHero : public AHeroBase
{
	GENERATED_BODY()

	//=====재 정의 함수=====
public:
	// Sets default values for this character's properties
	ATracerHero();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//=====변수=====
private:
protected:
public:
	//=====함수=====
protected:
public:
	//==고유 함수 영역==
private:
protected:
public:
};
