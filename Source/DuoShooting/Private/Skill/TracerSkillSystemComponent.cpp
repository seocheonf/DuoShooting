// Fill out your copyright notice in the Description page of Project Settings.


#include "DuoShooting/Public/Skill/TracerSkillSystemComponent.h"

#include "BlueprintEditor.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/TracerHero.h"

// Sets default values for this component's properties
UTracerSkillSystemComponent::UTracerSkillSystemComponent()
	: CurrentSkillState(ETracerSkillState::NONE)
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

	Owner = Cast<ATracerHero>(GetOwner());
	if (!Owner) { UE_LOG(LogTemp, Warning, TEXT("UTracerSkillSystemComponent에서 ATracerHero 타입의 Owner를 찾지 못함")); }
}


// Called every frame
void UTracerSkillSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (CurrentSkillState)
	{
	case ETracerSkillState::BLINK:
		TickBlink();
		RecordPoints();
		break;
	case ETracerSkillState::RECALL:
		TickRecall();
		break;
	default:
		RecordPoints();
		break;
	}
}

void UTracerSkillSystemComponent::SetupHeroInputInfo(UEnhancedInputComponent* enhancedInputComponent)
{
	//Super::SetupHeroInputInfo(enhancedInputComponent); // 이거 풀면 튕긴다

	enhancedInputComponent->BindAction(IA_Blink, ETriggerEvent::Started, this,
	                                   &UTracerSkillSystemComponent::InputBlink);
	enhancedInputComponent->BindAction(IA_Recall, ETriggerEvent::Started, this,
	                                   &UTracerSkillSystemComponent::InputRecall);
}

void UTracerSkillSystemComponent::InputBlink(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("InputBlink"));

	ActivateBlink();
}

void UTracerSkillSystemComponent::InputRecall(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("InputRecall"));

	CurrentSkillState = ETracerSkillState::RECALL;
}

// 점멸 활성화
void UTracerSkillSystemComponent::ActivateBlink()
{
	// 이미 스킬이 실행중이면 리턴
	if (CurrentSkillState != ETracerSkillState::NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("트레이서 Blink 활성화 불가 - CurrentSkillState가 %s"),
		       *UEnum::GetValueAsString(CurrentSkillState));
		return;
	}

	CurrentSkillState = ETracerSkillState::BLINK;

	// 일정 시간 뒤 비활성화하기
	GetWorld()->GetTimerManager().SetTimer(BlinkTimerHandle, this, &UTracerSkillSystemComponent::DeactivateBlink,
	                                       BlinkDuration,
	                                       false);
}

// 프레임별 점멸 로직
void UTracerSkillSystemComponent::TickBlink()
{
	// 점멸 방향
	FVector BlinkDirection;

	// 캐릭터가 이동하고 있다면 해당 방향으로
	FVector CurrentVelocity = Owner->GetCharacterMovement()->Velocity;
	CurrentVelocity.Z = 0.0f;
	if (!CurrentVelocity.IsNearlyZero())
	{
		BlinkDirection = CurrentVelocity.GetSafeNormal();
	}
	// 캐릭터 이동이 없으면 그냥 앞방향으로
	else
	{
		BlinkDirection = Owner->GetActorForwardVector();
		BlinkDirection.Z = 0.0f;
		BlinkDirection.Normalize();
	}

	// 캐릭터가 땅에 닿아 있다면 경사로까지 고려해서 밀자
	const FFindFloorResult& Floor = Owner->GetCharacterMovement()->CurrentFloor;
	if (Floor.IsWalkableFloor())
	{
		FVector FloorNormal = Floor.HitResult.ImpactNormal;
		FVector SlopeRight = FVector::CrossProduct(FloorNormal, BlinkDirection);
		BlinkDirection = FVector::CrossProduct(SlopeRight, FloorNormal);
		BlinkDirection.Normalize();
	}

	// 움직이기
	FHitResult Hit;
	Owner->GetCharacterMovement()->SafeMoveUpdatedComponent(
		BlinkDirection * BlinkDistance,
		Owner->GetActorRotation(),
		true,
		Hit
	);
}

// 점멸 비활성화
void UTracerSkillSystemComponent::DeactivateBlink()
{
	CurrentSkillState = ETracerSkillState::NONE;
}

void UTracerSkillSystemComponent::RecordPoints()
{
}

void UTracerSkillSystemComponent::ActivateRecall()
{
}

void UTracerSkillSystemComponent::TickRecall()
{
}

void UTracerSkillSystemComponent::DeactivateRecall()
{
}
