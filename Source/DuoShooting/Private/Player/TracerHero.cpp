// Fill out your copyright notice in the Description page of Project Settings.


#include "DuoShooting/Public/Player/TracerHero.h"


// Sets default values
ATracerHero::ATracerHero()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATracerHero::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATracerHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATracerHero::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

