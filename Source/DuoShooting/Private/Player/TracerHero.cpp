// Fill out your copyright notice in the Description page of Project Settings.


#include "DuoShooting/Public/Player/TracerHero.h"
#include "Skill/TracerSkillSystemComponent.h"
#include "EnhancedInputComponent.h"


// Sets default values
ATracerHero::ATracerHero()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetSkillSystemComponent(CreateDefaultSubobject<UTracerSkillSystemComponent>("SkillSystemComp"));
}

// Called when the game starts or when spawned
void ATracerHero::BeginPlay()
{
	Super::BeginPlay();

	TracerSkillSystemComp = Cast<UTracerSkillSystemComponent>(GetSkillSystemComponent());
}

void ATracerHero::InputMove(const struct FInputActionValue& value)
{
	if (TracerSkillSystemComp->GetCurrentSkillState() == ETracerSkillState::BLINK) return;
	if (TracerSkillSystemComp->GetCurrentSkillState() == ETracerSkillState::RECALL) return;
	
	Super::InputMove(value);
}

void ATracerHero::InputLook(const struct FInputActionValue& value)
{
	if (TracerSkillSystemComp->GetCurrentSkillState() == ETracerSkillState::BLINK) return;
	if (TracerSkillSystemComp->GetCurrentSkillState() == ETracerSkillState::RECALL) return;

	Super::InputLook(value);
}

void ATracerHero::InputJump(const struct FInputActionValue& value)
{
	if (TracerSkillSystemComp->GetCurrentSkillState() == ETracerSkillState::BLINK) return;
	if (TracerSkillSystemComp->GetCurrentSkillState() == ETracerSkillState::RECALL) return;

	Super::InputJump(value);
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
