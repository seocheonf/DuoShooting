// Fill out your copyright notice in the Description page of Project Settings.

#include "DuoShooting/Public/Skill/SombraSkillSystemComponent.h"

#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Player/HeroBase.h"

// Sets default values for this component's properties
USombraSkillSystemComponent::USombraSkillSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	
	//솜브라 스킬 입력 불러오기
	//EMP
	ConstructorHelpers::FObjectFinder<UInputAction> ia_emp(TEXT("/Script/EnhancedInput.InputAction'/Game/DuoShooting/Inputs/Sombra/IA_SombraEMP.IA_SombraEMP'"));
	if (ia_emp.Succeeded())
	{
		IA_EMP = ia_emp.Object;
	}
	//Hack
	ConstructorHelpers::FObjectFinder<UInputAction> ia_hack(TEXT("/Script/EnhancedInput.InputAction'/Game/DuoShooting/Inputs/Sombra/IA_SombraHack.IA_SombraHack'"));
	if (ia_emp.Succeeded())
	{
		IA_Hack = ia_hack.Object;
	}
	//Virus
	ConstructorHelpers::FObjectFinder<UInputAction> ia_virus(TEXT("/Script/EnhancedInput.InputAction'/Game/DuoShooting/Inputs/Sombra/IA_SombraVirus.IA_SombraVirus'"));
	if (ia_virus.Succeeded())
	{
		IA_Virus = ia_virus.Object;
	}
	//Translocator
	ConstructorHelpers::FObjectFinder<UInputAction> ia_translocator(TEXT("/Script/EnhancedInput.InputAction'/Game/DuoShooting/Inputs/Sombra/IA_SombraTranslocator.IA_SombraTranslocator'"));
	if (ia_translocator.Succeeded())
	{
		IA_Translocator = ia_translocator.Object;
	}
	
	//솜브라 스킬에 대한 IMC 불러오기
	ConstructorHelpers::FObjectFinder<UInputMappingContext> imc(TEXT("'/Game/DuoShooting/Inputs/Sombra/IMC_Sombra.IMC_Sombra'"));
	if (imc.Succeeded())
	{
		IMC_SkillSystem = imc.Object; 
	}
	
}


// Called when the game starts
void USombraSkillSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USombraSkillSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USombraSkillSystemComponent::SetupHeroInputInfo(class UEnhancedInputComponent* enhancedInputComponent)
{
	enhancedInputComponent->BindAction(IA_EMP, ETriggerEvent::Started, this, &USombraSkillSystemComponent::OnEMP);
	enhancedInputComponent->BindAction(IA_Hack, ETriggerEvent::Triggered, this, &USombraSkillSystemComponent::OnHack);
	enhancedInputComponent->BindAction(IA_Virus, ETriggerEvent::Started, this, &USombraSkillSystemComponent::OnVirus);
	enhancedInputComponent->BindAction(IA_Translocator, ETriggerEvent::Started, this, &USombraSkillSystemComponent::OnTranslocator);
}

void USombraSkillSystemComponent::OnEMP(const struct FInputActionValue& value)
{
	
}

void USombraSkillSystemComponent::OnHack(const struct FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("OnHack"));
}

void USombraSkillSystemComponent::OnVirus(const struct FInputActionValue& value)
{
}

void USombraSkillSystemComponent::OnTranslocator(const struct FInputActionValue& value)
{
	TargetPlayer->GetActorLocation();
}

