// Fill out your copyright notice in the Description page of Project Settings.

#include "DuoShooting/Public/Player/SombraHero.h"

#include "Skill/SombraSkillSystemComponent.h"

// Sets default values
ASombraHero::ASombraHero()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASombraHero::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASombraHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASombraHero::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

