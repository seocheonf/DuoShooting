// Fill out your copyright notice in the Description page of Project Settings.


#include "DuoShooting/Public/Skill/TracerSkillSystemComponent.h"

#include "BlueprintEditor.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/TracerHero.h"


FTransformSnapshot::FTransformSnapshot()
	: Location(FVector::ZeroVector), Rotation(FRotator::ZeroRotator)
{
}

FTransformSnapshot::FTransformSnapshot(const FVector& Location, const FRotator& Rotation)
	: Location(FVector::ZeroVector), Rotation(FRotator::ZeroRotator)
{
}

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

	Records.Init(RecordLength);
}


// Called when the game starts
void UTracerSkillSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ATracerHero>(GetOwner());
	if (!Owner) { UE_LOG(LogTemp, Warning, TEXT("UTracerSkillSystemComponent에서 ATracerHero 타입의 Owner를 찾지 못함")); }

	GetWorld()->GetTimerManager().SetTimer(RecallTimerHandle, this, &UTracerSkillSystemComponent::RecordPoints,
	                                       RecordInterval, true);
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
	ActivateBlink();
}

void UTracerSkillSystemComponent::InputRecall(const FInputActionValue& value)
{
	CurrentSkillState = ETracerSkillState::RECALL;
}

// 점멸 활성화
void UTracerSkillSystemComponent::ActivateBlink()
{
	// 이미 스킬이 실행중이면 리턴
	if (CurrentSkillState != ETracerSkillState::NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("트레이서 점멸 활성화 불가 - CurrentSkillState가 %s"),
		       *UEnum::GetValueAsString(CurrentSkillState));
		return;
	}

	CurrentSkillState = ETracerSkillState::BLINK;
	UE_LOG(LogTemp, Warning, TEXT("점멸 활성화"));

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

	// 일단, Z방향을 없앤 점별방향을 계산하자
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

	// 단 캐릭터가 땅에 닿아 있다면 경사로까지 고려해서 Z축방향을 추가
	const FFindFloorResult& Floor = Owner->GetCharacterMovement()->CurrentFloor;
	if (Floor.IsWalkableFloor())
	{
		FVector FloorNormal = Floor.HitResult.ImpactNormal;
		FVector SlopeRight = FVector::CrossProduct(FloorNormal, BlinkDirection);
		BlinkDirection = FVector::CrossProduct(SlopeRight, FloorNormal);
		BlinkDirection.Normalize();
	}

	// 실제로 움직이기
	FHitResult Hit;
	Owner->GetCharacterMovement()->SafeMoveUpdatedComponent(
		BlinkDirection * BlinkSpeed * GetWorld()->GetDeltaSeconds(),
		Owner->GetActorRotation(),
		true,
		Hit
	);
}

// 점멸 비활성화
void UTracerSkillSystemComponent::DeactivateBlink()
{
	CurrentSkillState = ETracerSkillState::NONE;

	UE_LOG(LogTemp, Warning, TEXT("점멸 비활성화"));
}

// 큐에 위치 기록
void UTracerSkillSystemComponent::RecordPoints()
{
	UE_LOG(LogTemp, Warning, TEXT("트레이서는 지나왔던 길들을 기록 중"));

	FTransformSnapshot snapshot(Owner->GetActorLocation(), Owner->GetActorRotation());
	Records.Push_Back(snapshot);
}

// 시간 역행 활성화
void UTracerSkillSystemComponent::ActivateRecall()
{
	// 이미 스킬이 실행중이면 리턴
	if (CurrentSkillState != ETracerSkillState::NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("트레이서 시간 역행 활성화 불가 - CurrentSkillState가 %s"),
			   *UEnum::GetValueAsString(CurrentSkillState));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("시간역행 활성화"));

	// 시간역행용 타이머로 전환
	GetWorld()->GetTimerManager().ClearTimer(RecallTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(RecallTimerHandle, this, &UTracerSkillSystemComponent::RecallPoints,
								   RecallInterval / RecordLength, true);

	CurrentSkillState = ETracerSkillState::RECALL;
}

// 큐에서 위치 꺼내서 이동하기
void UTracerSkillSystemComponent::RecallPoints()
{

	bool isEmpty;
	FTransformSnapshot snapshot = Records.Pop_Back(isEmpty);
	Owner->SetActorLocationAndRotation(snapshot.Location, snapshot.Rotation);
	UE_LOG(LogTemp, Warning, TEXT("지나왔던 길들을 꺼내 보기 (%s, %s)"), *snapshot.Location.ToString(), *snapshot.Rotation.ToString());

	if (isEmpty)
	{
		DeactivateRecall();
	}
}

// 시간 역행 비활성화
void UTracerSkillSystemComponent::DeactivateRecall()
{
	CurrentSkillState = ETracerSkillState::NONE;
	
	UE_LOG(LogTemp, Warning, TEXT("시간역행 비활성화"));

	// 시간기록용 타이머로 전환
	GetWorld()->GetTimerManager().ClearTimer(RecallTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(RecallTimerHandle, this, &UTracerSkillSystemComponent::RecordPoints,
									   RecordInterval, true);
}
