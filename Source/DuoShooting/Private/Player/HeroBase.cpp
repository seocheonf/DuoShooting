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

void AHeroBase::AddCurrentHeroState(EHeroState newState)
{
	int32 newStateBitmask = 1 << (int8)newState;
	CurrentHeroState |= newStateBitmask;
}

void AHeroBase::RemoveCurrentHeroState(EHeroState oldState)
{
	int32 oldStateBitmask = 1 << (int8)oldState;
	CurrentHeroState &= (~oldStateBitmask);
}

void AHeroBase::InitConstructor()
{
	SkillSystemComp = InitSkillSystemComponent();
}

USkillSystemComponent* AHeroBase::GetSkillSystemComponent() const
{
	return SkillSystemComp;
}

TArray<EHeroState> AHeroBase::GetCurrentHeroState()
{
	TArray<EHeroState> result;
	for (int8 i = 0; i < (int8)EHeroState::LastIndex; i++)
	{
		if (CurrentHeroState & (1 << i))
			result.Add((EHeroState)i);
	}
	return result;
}