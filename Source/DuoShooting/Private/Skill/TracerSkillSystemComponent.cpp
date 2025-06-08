// Fill out your copyright notice in the Description page of Project Settings.


#include "DuoShooting/Public/Skill/TracerSkillSystemComponent.h"

#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "Attack/HitscanEmitterComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/TracerHero.h"
#include "Skill/TracerSkill/PulseBomb.h"


FTracerRecallInfo::FTracerRecallInfo()
	: Location(FVector::ZeroVector), ControlRotation(FVector2D::ZeroVector), Health(0.0f)
{
}

FTracerRecallInfo::FTracerRecallInfo(const FVector& location, float controlRot_Pitch, float controlRot_Yaw,
                                     float health)
{
	Location = location;
	ControlRotation.X = controlRot_Pitch;
	ControlRotation.Y = controlRot_Yaw;
	Health = health;
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
			TEXT("'/Game/DuoShooting/Inputs/Tracer/IA_TracerPulseBomb.IA_TracerPulseBomb'"));
		if (TempIA.Succeeded()) { IA_PulseBomb = TempIA.Object; }
	}
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
	// 발사체 원본
	{
		ConstructorHelpers::FClassFinder<APulseBomb> TempClass(
			TEXT("'/Game/DuoShooting/Blueprints/Characters/Skill/Tracer/BP_PulseBomb.BP_PulseBomb_C'"));
		if (TempClass.Succeeded()) { PulseBombFactory = TempClass.Class; }
	}

	Records.Init(RecordLength);

	SetIsReplicated(true);
}

// Called when the game starts
void UTracerSkillSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ATracerHero>(GetOwner());
	if (!Owner) { UE_LOG(LogTemp, Warning, TEXT("UTracerSkillSystemComponent에서 ATracerHero 타입의 Owner를 찾지 못함")); }

	GetWorld()->GetTimerManager().SetTimer(RecallTimerHandle, this, &UTracerSkillSystemComponent::RecordInfo,
	                                       RecordInterval, true);

	RecallStepDuration = RecallInterval / RecordLength;
	UE_LOG(LogTemp, Warning, TEXT("RecallStepDuration : %f"), RecallStepDuration);
}

// 혹시모를 타이머 
void UTracerSkillSystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(RecallTimerHandle);
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
		TickRecall(DeltaTime);
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
	enhancedInputComponent->BindAction(IA_PulseBomb, ETriggerEvent::Started, this,
								   &UTracerSkillSystemComponent::InputPulseBomb);
}

void UTracerSkillSystemComponent::InputBlink(const FInputActionValue& value)
{
	ActivateBlink();
}

void UTracerSkillSystemComponent::InputRecall(const FInputActionValue& value)
{
	ActivateRecall();
}

void UTracerSkillSystemComponent::InputPulseBomb(const struct FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("Input Pulse Bomb"));
	ThrowPulseBomb();
}

void UTracerSkillSystemComponent::ThrowPulseBomb()
{
	// 시간역행중에는 불가
	if (CurrentSkillState == ETracerSkillState::RECALL) return;

	FVector TempStart;
	TempStart = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 100;
	APulseBomb* bomb = GetWorld()->SpawnActor<APulseBomb>(PulseBombFactory, TempStart, Owner->GetActorRotation());

	// 일단 앞의 적당한 방향에 던져보는 걸로
	FVector TempDir = Owner->GetActorForwardVector();
	TempDir.Z = TempDir.Z + 1.0f;
	if (bomb)
		bomb->Launch(TempDir, 500.0f, Owner->Controller);
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

	// 일단, Z방향을 없앤 점멸방향 계산
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

	// 일정 시간 뒤 비활성화 예약
	GetWorld()->GetTimerManager().SetTimer(BlinkTimerHandle, this, &UTracerSkillSystemComponent::DeactivateBlink,
	                                       BlinkDuration,
	                                       false);

	TestStartLocation = Owner->GetActorLocation();
}

// 프레임별 점멸 로직
void UTracerSkillSystemComponent::TickBlink()
{
	FVector SlopedBlinkDirection = BlinkDirection;

	// 캐릭터가 땅에 닿아 있다면 경사로까지 고려해서 Z축방향을 추가
	const FFindFloorResult& Floor = Owner->GetCharacterMovement()->CurrentFloor;
	if (Floor.IsWalkableFloor())
	{
		FVector FloorNormal = Floor.HitResult.ImpactNormal;
		FVector SlopeRight = FVector::CrossProduct(FloorNormal, SlopedBlinkDirection);
		SlopedBlinkDirection = FVector::CrossProduct(SlopeRight, FloorNormal);
		SlopedBlinkDirection.Normalize();
	}
	//UE_LOG(LogTemp, Warning, TEXT("SlopedBlinkDirection %s"), *SlopedBlinkDirection.ToString());

	// 실제로 움직이기
	FHitResult Hit;
	Owner->GetCharacterMovement()->SafeMoveUpdatedComponent(
		SlopedBlinkDirection * BlinkSpeed * GetWorld()->GetDeltaSeconds(),
		Owner->GetActorRotation(),
		true,
		Hit
	);
}

// 점멸 비활성화
void UTracerSkillSystemComponent::DeactivateBlink()
{
	CurrentSkillState = ETracerSkillState::NONE;
	UE_LOG(LogTemp, Warning, TEXT("트레이서 점멸 통계: %f의 거리 이동"),
	       FVector::Dist(TestStartLocation, Owner->GetActorLocation()));
	UE_LOG(LogTemp, Warning, TEXT("점멸 비활성화"));
}

// 큐에 위치 기록
void UTracerSkillSystemComponent::RecordInfo()
{
	FTracerRecallInfo snapshot(Owner->GetActorLocation(), Owner->GetControlRotation().Pitch,
	                           Owner->GetControlRotation().Yaw, Owner->GetHealth());
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
	Owner->SetMeshVisibility(false);
	Owner->SetCollisionEnable(false);
	// Owner->GetCamera()->bUsePawnControlRotation = false;
	// Owner->bUseControllerRotationYaw = false;

	// 시간역행용 타이머로 전환
	GetWorld()->GetTimerManager().ClearTimer(RecallTimerHandle);
	RecallInfo();
	GetWorld()->GetTimerManager().SetTimer(RecallTimerHandle, this, &UTracerSkillSystemComponent::RecallInfo,
		RecallStepDuration, true);
}

void UTracerSkillSystemComponent::TickRecall(float DeltaTime)
{
	// 델타타임 쌓기
	TimeSinceLastRecallInterval = FMath::Clamp(TimeSinceLastRecallInterval + DeltaTime, 0.0f, RecallStepDuration);
	float alpha = FMath::Clamp(TimeSinceLastRecallInterval / RecallStepDuration, 0.0f, 1.0f);

	// 위치 보간 적용
	FVector interpLocation = FMath::Lerp(IntervalOrigin.Location, IntervalTarget.Location, alpha);
	Owner->SetActorLocation(interpLocation);

	// 컨트롤 회전값 보간 적용
	FRotator currentRot(IntervalOrigin.ControlRotation.X, IntervalOrigin.ControlRotation.Y, 0.f);
	FQuat currentQuat = currentRot.Quaternion();
	FRotator targetRot(IntervalTarget.ControlRotation.X, IntervalTarget.ControlRotation.Y, 0.f);
	FQuat targetQuat = targetRot.Quaternion();
	FQuat interpQuat = FQuat::Slerp(currentQuat, targetQuat, alpha);

	if (AController* ownerController = Owner->GetController())
		ownerController->SetControlRotation(interpQuat.Rotator());
}

// 큐에서 위치 꺼내기
void UTracerSkillSystemComponent::RecallInfo()
{
	bool valid;

	TimeSinceLastRecallInterval = 0.0f;

	// 보간을 위해 IntervalOrigin에 현재값 저장
	IntervalOrigin.Location = Owner->GetActorLocation();
	IntervalOrigin.ControlRotation = FVector2D(Owner->GetControlRotation().Pitch, Owner->GetControlRotation().Yaw);
	IntervalOrigin.Health = Owner->GetHealth();

	// 보간을 위한 새로운 목표값 저장
	IntervalTarget = Records.Pop_Back(valid);

	if (!valid) DeactivateRecall();
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
	Owner->SetMeshVisibility(true);
	Owner->SetCollisionEnable(true);

	// 시간기록용 타이머로 전환
	GetWorld()->GetTimerManager().ClearTimer(RecallTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(RecallTimerHandle, this, &UTracerSkillSystemComponent::RecordInfo,
	                                       RecordInterval, true);
}

ETracerSkillState UTracerSkillSystemComponent::GetCurrentSkillState() const { return CurrentSkillState; }