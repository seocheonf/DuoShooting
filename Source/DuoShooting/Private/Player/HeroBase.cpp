// Fill out your copyright notice in the Description page of Project Settings.


#include "InputActionValue.h"
#include "DuoShooting/Public/Player/HeroBase.h"

#include "EnhancedInputComponent.h"
#include "DuoShooting/Public/Skill/SkillSystemComponent.h"


// Sets default values
AHeroBase::AHeroBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitConstructor();
}

// Called when the game starts or when spawned
void AHeroBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHeroBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AHeroBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//스킬 시스템에 Input정보 넘기기
	InitSkillSystemInput(PlayerInputComponent);
}

void AHeroBase::InitSkillSystemInput(class UInputComponent* playerInputComponent)
{
	if (auto* input = Cast<UEnhancedInputComponent>(playerInputComponent))
	{
		if (nullptr != SkillSystemComp)
			SkillSystemComp->SetupHeroInfo(this, input);
		else
			UE_LOG(LogTemp, Error, TEXT("SkillSystemComp is null!!!!! You must initialize SkillSystemComp!!!!!"));
	}
}

void AHeroBase::InitConstructor()
{
	SkillSystemComp = InitSkillSystemComponent();
}

USkillSystemComponent* AHeroBase::GetSkillSystemComponent() const
{
	return SkillSystemComp;
}

enum EHeroState AHeroBase::GetCurrentState()
{
	return CurrentState;
}
