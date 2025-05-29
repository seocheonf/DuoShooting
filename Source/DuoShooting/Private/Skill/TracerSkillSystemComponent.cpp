// Fill out your copyright notice in the Description page of Project Settings.


#include "DuoShooting/Public/Skill/TracerSkillSystemComponent.h"

#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "Attack/HitscanEmitterComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/TracerHero.h"


FTransformSnapshot::FTransformSnapshot()
	: Location(FVector::ZeroVector), Rotation(FRotator::ZeroRotator)
{
}

FTransformSnapshot::FTransformSnapshot(const FVector& Location, const FRotator& Rotation)
	: Location(Location), Rotation(Rotation)
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
		break;
	case ETracerSkillState::RECALL:
		break;
	default:
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
	ActivateRecall();
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
	FTransformSnapshot snapshot(Owner->GetActorLocation(), Owner->GetControlRotation());
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

	CurrentSkillState = ETracerSkillState::RECALL;
	UE_LOG(LogTemp, Warning, TEXT("시간역행 활성화"));

	// 컴포넌트 설정값들 끄기
	Owner->GetCharacterMovement()->DisableMovement();
	Owner->GetHitscanEmitter()->Disable();
	// Owner->GetCamera()->bUsePawnControlRotation = false;
	// Owner->bUseControllerRotationYaw = false;

	// 시간역행용 타이머로 전환
	GetWorld()->GetTimerManager().ClearTimer(RecallTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(RecallTimerHandle, this, &UTracerSkillSystemComponent::RecallPoints,
	                                       RecallInterval / RecordLength, true);
}

// 큐에서 위치 꺼내서 이동하기
void UTracerSkillSystemComponent::RecallPoints()
{
	bool valid;
	FTransformSnapshot snapshot = Records.Pop_Back(valid);

	if (valid)
	{
		Owner->SetActorLocation(snapshot.Location);
		AController* ownerController = Owner->GetController();
		if (ownerController)
			ownerController->SetControlRotation(snapshot.Rotation);
	}
	else
	{
		DeactivateRecall();
	}
}

// 시간 역행 비활성화
void UTracerSkillSystemComponent::DeactivateRecall()
{
	CurrentSkillState = ETracerSkillState::NONE;

	UE_LOG(LogTemp, Warning, TEXT("시간역행 비활성화"));

	// 혹시나 뱉어내지 않은 기록이 남아있다면 제거
	Records.Clear();

	// 컴포넌트 설정값들 켜기
	if (Owner->GetCharacterMovement()->IsMovingOnGround())
		Owner->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	else
		Owner->GetCharacterMovement()->SetMovementMode(MOVE_Falling);

	Owner->GetHitscanEmitter()->Enable();

	// 시간기록용 타이머로 전환
	GetWorld()->GetTimerManager().ClearTimer(RecallTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(RecallTimerHandle, this, &UTracerSkillSystemComponent::RecordPoints,
	                                       RecordInterval, true);
}

ETracerSkillState UTracerSkillSystemComponent::GetCurrentSkillState() const { return CurrentSkillState; }
