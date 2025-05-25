// Fill out your copyright notice in the Description page of Project Settings.


#include "DuoShooting/Public/Skill/TracerSkillSystemComponent.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"

// Sets default values for this component's properties
UTracerSkillSystemComponent::UTracerSkillSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 리소스 찾기
	// IMC 등록
	{
		ConstructorHelpers::FObjectFinder<UInputMappingContext> TempIMC(
			TEXT("'/Game/DuoShooting/Inputs/Tracer/IMC_Tracer.IMC_Tracer'"));
		if (TempIMC.Succeeded()) { IMC_SkillSystem = TempIMC.Object; }
	}
	// IA 등록
	{
		ConstructorHelpers::FObjectFinder<UInputAction> TempIA(
			TEXT("'/Game/DuoShooting/Inputs/Tracer/IA_TracerBlink.IA_TracerBlink'"));
		if (TempIA.Succeeded()) { IA_Blink = TempIA.Object; }
	}
	{
		ConstructorHelpers::FObjectFinder<UInputAction> TempIA(
			TEXT("'/Game/DuoShooting/Inputs/Tracer/IA_TracerRecall.IA_TracerRecall'"));
		if (TempIA.Succeeded()) { IA_Recall = TempIA.Object; }
	}
}


// Called when the game starts
void UTracerSkillSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UTracerSkillSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTracerSkillSystemComponent::SetupHeroInputInfo(UEnhancedInputComponent* enhancedInputComponent)
{
	//Super::SetupHeroInputInfo(enhancedInputComponent); // 이거 풀면 튕긴다

	enhancedInputComponent->BindAction(IA_Blink, ETriggerEvent::Started, this, &UTracerSkillSystemComponent::InputBlink);
	enhancedInputComponent->BindAction(IA_Recall, ETriggerEvent::Started, this, &UTracerSkillSystemComponent::InputRecall);
}

void UTracerSkillSystemComponent::InputBlink(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("InputBlink"));
}

void UTracerSkillSystemComponent::InputRecall(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("InputRecall"));
}
