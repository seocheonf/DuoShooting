// Fill out your copyright notice in the Description page of Project Settings.


#include "DuoShooting/Public/Skill/SkillSystemComponent.h"


// Sets default values for this component's properties
USkillSystemComponent::USkillSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USkillSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USkillSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USkillSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void USkillSystemComponent::SetupHeroInfo(class AHeroBase* targetPlayer,
	class UEnhancedInputComponent* enhancedInputComponent)
{
	TargetPlayer = targetPlayer;
	SetupHeroInputInfo(enhancedInputComponent);
}

