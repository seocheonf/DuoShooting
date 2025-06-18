// Fill out your copyright notice in the Description page of Project Settings.


#include "DuoShooting/Public/Skill/TracerSkillSystemComponent.h"

#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "NiagaraComponent.h"
#include "Attack/HitscanEmitterComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TracerHero.h"
#include "Skill/TracerSkill/PulseBomb.h"
#include "Tool/CoolTimerManagerComponent.h"
#include "UI/MiniSkillCoolTimeUI.h"


FTracerRecallInfo::FTracerRecallInfo()
	: Location(FVector::ZeroVector), Health(0.0f), ControlRotation(FVector2D::ZeroVector)
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
	// 사운드
	{
		ConstructorHelpers::FObjectFinder<USoundBase> TempSound(
			TEXT("'/Game/DuoShooting/Sounds/TracerBlinkSound.TracerBlinkSound'"));
		if (TempSound.Succeeded()) { BlinkSound = TempSound.Object; }
	}
	{
		ConstructorHelpers::FObjectFinder<USoundBase> TempSound(
			TEXT("'/Game/DuoShooting/Sounds/TracerRecallSound_Start.TracerRecallSound_Start'"));
		if (TempSound.Succeeded()) { RecallSound_Start = TempSound.Object; }
	}
	{
		ConstructorHelpers::FObjectFinder<USoundBase> TempSound(
			TEXT("'/Game/DuoShooting/Sounds/TracerRecallSound_End.TracerRecallSound_End'"));
		if (TempSound.Succeeded()) { RecallSound_End = TempSound.Object; }
	}

	SetIsReplicated(true);

	//==김형모
	//스킬 아이콘 받아오기
	{
		ConstructorHelpers::FObjectFinder<UTexture2D> blinkTexture(TEXT("/Script/Engine.Texture2D'/Game/DuoShooting/Sprites/SkillIcons/TracerBlink.TracerBlink'"));
		if (blinkTexture.Succeeded())
		{
			OriginBlinkTexture2D = blinkTexture.Object;
		}
		ConstructorHelpers::FObjectFinder<UTexture2D> recallTexture(TEXT("/Script/Engine.Texture2D'/Game/DuoShooting/Sprites/SkillIcons/TracerRecall.TracerRecall'"));
		{
			OriginRecallTexture2D = recallTexture.Object;
		}
		ConstructorHelpers::FObjectFinder<UTexture2D> pulseBombTexture(TEXT("/Script/Engine.Texture2D'/Game/DuoShooting/Sprites/SkillIcons/TracerBomb.TracerBomb'"));
		{
			OriginPulseBombTexture2D = pulseBombTexture.Object;
		}
	}
	//트레이서 필요 스킬 UI 받아오기
	{
		ConstructorHelpers::FClassFinder<UMiniSkillCoolTimeUI> miniUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/DuoShooting/UIs/WBP_MiniSkillCoolTimeUI.WBP_MiniSkillCoolTimeUI_C'"));
		if (miniUI.Succeeded())
		{
			OriginBlinkCoolTimeUI = miniUI.Class;
		}
		// ConstructorHelpers::FClassFinder<UTracerSkillSystemUI> tracerUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/DuoShooting/UIs/WBP_TracerSkillSystemUI.WBP_TracerSkillSystemUI_C'"));
		// if (tracerUI.Succeeded())
		// {
		// 	OriginTracerSkillUI = tracerUI.Class;
		// }
	}
	
}

// Called when the game starts
void UTracerSkillSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ATracerHero>(GetOwner());
	if (!Owner) { UE_LOG(LogTemp, Warning, TEXT("UTracerSkillSystemComponent에서 ATracerHero 타입의 Owner를 찾지 못함")); }

	// 시간역행용 기록 시작
	Records.Init(RecordLength);
	GetWorld()->GetTimerManager().SetTimer(RecallTimerHandle, this, &UTracerSkillSystemComponent::RecordInfo,
		RecordInterval, true);
	RecallStepDuration = RecallInterval / RecordLength;

	//==김형모
	if (TargetPlayer->IsLocallyControlled())
	{
		BlinkIconIndex = AddSkillUI(OriginBlinkTexture2D, FText::FromString(TEXT("좌SHIFT")));
		RecallIconIndex = AddSkillUI(OriginRecallTexture2D, FText::FromString(TEXT("E")));
		PulseBombIconIndex = AddSkillUI(OriginPulseBombTexture2D, FText::FromString(TEXT("Q")));

		BlinkCoolTimeUI = CreateWidget<UMiniSkillCoolTimeUI>(GetWorld(), OriginBlinkCoolTimeUI);
		AddAdditionalSkillUI(BlinkIconIndex, BlinkCoolTimeUI);
	}
	else
	{
		//임시로 서버 데이터 갱신.
		BlinkIconIndex = 0;
		RecallIconIndex = 1;
		PulseBombIconIndex = 2;
	}
}

void UTracerSkillSystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 혹시모를 타이머 해제
	GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(RecallTimerHandle);
	
	Super::EndPlay(EndPlayReason);
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

	//DebugInfo();
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
	UE_LOG(LogTemp, Warning, TEXT("Tracer Input Blink"));
	
	// 이미 스킬이 실행중이면 리턴
	if (CurrentSkillState != ETracerSkillState::NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("트레이서 점멸 활성화 불가 - CurrentSkillState가 %s"),
			*UEnum::GetValueAsString(CurrentSkillState));
		return;
	}

	//CurrentSkillState = ETracerSkillState::BLINK;
	UE_LOG(LogTemp, Warning, TEXT("점멸 활성화"));

	/// 진행방향을 설정하여 서버에게 전달
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

	BlinkStartPos = Owner->GetActorLocation();

	ServerRPC_BlinkStart(BlinkStartPos, BlinkDirection);

	UGameplayStatics::PlaySoundAtLocation(this, BlinkSound, Owner->GetActorLocation());
	if (Owner) Owner->GetBlinkNiagaraComponent()->Activate();
}

void UTracerSkillSystemComponent::InputPulseBomb(const struct FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("Tracer Input PulseBomb"));
	
	// 시간역행중에는 불가
	if (CurrentSkillState == ETracerSkillState::RECALL) return;

	ServerRPC_ThrowPulseBomb();
}

// 프레임별 점멸 로직
void UTracerSkillSystemComponent::TickBlink()
{
	// 서버에서만 실행
	if (!Owner->HasAuthority()) return;

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
	MultiRPC_BlinkEnd();
	ClientRPC_BlinkEnd();
}

// 클라이언트 사이드 시간역행 활성화
void UTracerSkillSystemComponent::InputRecall(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("Tracer Input Recall"));

	// 개인적으로 보일 이펙트
	UGameplayStatics::PlaySoundAtLocation(this, RecallSound_Start, Owner->GetActorLocation(), 0.4f);
	if (Owner) Owner->GetRecallNiagaraComponent()->Activate();
	
	// 이미 스킬이 실행중이면 리턴
	if (CurrentSkillState != ETracerSkillState::NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("트레이서 시간 역행 활성화 불가 - CurrentSkillState가 %s"),
			*UEnum::GetValueAsString(CurrentSkillState));
		return;
	}

	// 서버에게 시간역행 시작 요청
	ServerRPC_RecallStart();
}

// 서버사이드 시간역행 활성화
void UTracerSkillSystemComponent::ServerRPC_RecallStart_Implementation()
{

	//=김형모===
	//사용 금지 상태라면 무시
	if (!bRecall)
		return;
	//시간 역행을 시작하므로 일단 사용 금지 상태로 진입
	bRecall = false;
	//=======
	
	// 모두가 시간역행을 시작하게 함
	MultiRPC_RecallStart();
}

void UTracerSkillSystemComponent::ActivateRecall()
{
	CurrentSkillState = ETracerSkillState::RECALL;

	// 시간역행용 타이머로 전환
	GetWorld()->GetTimerManager().ClearTimer(RecallTimerHandle);
	RecallInfo();
	GetWorld()->GetTimerManager().SetTimer(RecallTimerHandle, this, &UTracerSkillSystemComponent::RecallInfo,
		RecallStepDuration, true);

	ToggleRecallOwnerSettings(true);
}

// 평상시에 큐에 위치 기록
void UTracerSkillSystemComponent::RecordInfo()
{
	FTracerRecallInfo snapshot(Owner->GetActorLocation(), Owner->GetControlRotation().Pitch,
		Owner->GetControlRotation().Yaw, Owner->GetHealth());
	Records.Push_Back(snapshot);
}

void UTracerSkillSystemComponent::TickRecall(float DeltaTime)
{
	//// 서버에서만 작동
	//if (!Owner->HasAuthority()) return;

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

	// 서버쪽이 시간역행을 끝내면 모두에게 끝내라고 하기
	if (Owner->HasAuthority() && !valid)
	{
		//=김형모===
		//시간역행이 종료되었으니 쿨타임 갱신 시작
		{
			//아이콘 비활성화 및 게이지 초기화
			ClientRPC_SetSkillIconActivation(RecallIconIndex, false);
			
			FTimerHandle cool_TimerHandle;
			//틱 동안 할일은, UI갱신
			auto cool_tick = [&](float cool_deltaTime, float cool_currentTime)
			{
				//쿨타임 게이지 갱신
				ClientRPC_SetSkillCoolTimeUI(RecallIconIndex, cool_currentTime, RecallCoolTime);
				ClientRPC_SetSkillRemainTimeUI(RecallIconIndex, RecallCoolTime - cool_currentTime, false);
			};
			//마무리 시 할일은, UI원상복구 후, 사용가능하게 한다.
			auto cool_end = [&](float cool_existTime)
			{
				bRecall = true;
				//아이콘 활성화 및 게이지 초기화
				ClientRPC_SetSkillIconActivation(RecallIconIndex, true);
				ClientRPC_SetSkillRemainTimeUI(RecallIconIndex, 0, true);
				//스킬 온 사운드 재생
				ClientRPC_PlaySoundSkillOn();
			};

			FDoTimerTick cool_timerDo;
			FNotifyTimerEnd cool_timerEnd;
			cool_timerDo.BindLambda(cool_tick);
			cool_timerEnd.BindLambda(cool_end);

			//만들어둔 쿨타이머 기능을 활용
			CoolTimerManagerComp->RegisterCoolTimerAll(cool_TimerHandle, 0, RecallCoolTime, 0.003f, cool_timerDo, cool_timerEnd);
		}
		//=======
		
		MultiRPC_RecallEnd();
		ClientRPC_RecallEnd();
	}
}

void UTracerSkillSystemComponent::ToggleRecallOwnerSettings(bool isRecall)
{
	// 시간역행을 위한 컴포넌트 값 설정하기
	if (isRecall)
	{
		// Owner->GetCamera()->bUsePawnControlRotation = false;
		// Owner->bUseControllerRotationYaw = false;
		Owner->GetCharacterMovement()->DisableMovement();
		Owner->GetHitscanEmitter()->Disable();
		Owner->SetMeshVisibility(false);
		Owner->SetCollisionEnable(false);
		Owner->SetReplicateMovement(false);
	}
	// 시간역행에서 나왔을때 컴포넌트 설정 원래대로 되돌려놓기
	else
	{
		if (Owner->GetCharacterMovement()->IsMovingOnGround())
			Owner->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		else
			Owner->GetCharacterMovement()->SetMovementMode(MOVE_Falling);

		Owner->GetHitscanEmitter()->Enable();
		Owner->SetMeshVisibility(true);
		Owner->SetCollisionEnable(true);
		Owner->SetReplicateMovement(true);
	}
}

void UTracerSkillSystemComponent::DeactivateRecall()
{
	CurrentSkillState = ETracerSkillState::NONE;

	// 혹시나 뱉어내지 않은 기록이 남아있다면 제거
	Records.Clear();

	// 시간기록용 타이머로 전환
	GetWorld()->GetTimerManager().ClearTimer(RecallTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(RecallTimerHandle, this, &UTracerSkillSystemComponent::RecordInfo,
		RecordInterval, true);

	ToggleRecallOwnerSettings(false);
}

ETracerSkillState UTracerSkillSystemComponent::GetCurrentSkillState() const { return CurrentSkillState; }


void UTracerSkillSystemComponent::SetBlinkTimer()
{
	UE_LOG(LogTemp, Error, TEXT("%d"), BlinkCoolTimerHandle.IsValid());

	if (BlinkCoolTimerHandle.IsValid())
		return;
	
	//틱 동안 할일은, UI갱신
	auto cool_tick = [&](float cool_deltaTime, float cool_currentTime)
	{
		//쿨타임 게이지 갱신
		//ClientRPC_SetSkillCoolTimeUI(BlinkIconIndex, cool_currentTime, BlinkCoolTime);
		//블링크 아이콘 게이지 갱신
		ClientRPC_SetBlinkIconGage(cool_currentTime, BlinkCoolTime);
	};
	//마무리 시 할일은, UI원상복구 후, 사용가능하게 한다.
	auto cool_end = [&](float cool_existTime)
	{
		//블링크 카운트를 채워준다.
		BlinkCount++;
		//아이콘 활성화 및 게이지 초기화
		ClientRPC_SetSkillIconActivation(BlinkIconIndex, true);
		//블링크 아이콘 게이지 초기화
		ClientRPC_SetBlinkIconGage(0, 1);
		//블링크 카운트 초기화
		ClientRPC_SetBlinkCountUI(BlinkCount);
		//스킬 온 사운드 재생
		ClientRPC_PlaySoundSkillOn();

		if (BlinkCount < MaxBlinkCount)
			SetBlinkTimer();
	};

	FDoTimerTick cool_timerDo;
	FNotifyTimerEnd cool_timerEnd;
	cool_timerDo.BindLambda(cool_tick);
	cool_timerEnd.BindLambda(cool_end);

	//만들어둔 쿨타이머 기능을 활용
	CoolTimerManagerComp->RegisterCoolTimerAll(BlinkCoolTimerHandle, 0, BlinkCoolTime, 0.003f, cool_timerDo, cool_timerEnd);
}

void UTracerSkillSystemComponent::ClientRPC_SetBlinkCountUI_Implementation(int32 count)
{
	BlinkCoolTimeUI->SetCount(count);
}

void UTracerSkillSystemComponent::ClientRPC_SetBlinkIconGage_Implementation(float upper, float lower)
{
	BlinkCoolTimeUI->SetGage(upper, lower);
}

// void UTracerSkillSystemComponent::DoAfterTargetPlayerDie()
// {
// 	if (TargetPlayer != nullptr && TracerSkillUI != nullptr)
// 	{
// 		TracerSkillUI->RemoveFromParent();
// 	}
// }

void UTracerSkillSystemComponent::ClientRPC_BlinkEnd_Implementation()
{
	if (Owner) Owner->GetBlinkNiagaraComponent()->Deactivate();
}

void UTracerSkillSystemComponent::ClientRPC_RecallEnd_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, RecallSound_End, Owner->GetActorLocation());
	if (Owner) Owner->GetRecallNiagaraComponent()->Deactivate();
}

void UTracerSkillSystemComponent::MultiRPC_RecallStart_Implementation()
{
	ActivateRecall();
}

void UTracerSkillSystemComponent::MultiRPC_RecallEnd_Implementation()
{
	DeactivateRecall();
}

void UTracerSkillSystemComponent::MultiRPC_BlinkEnd_Implementation()
{
	CurrentSkillState = ETracerSkillState::NONE;
}

void UTracerSkillSystemComponent::MultiRPC_BlinkStart_Implementation(FVector StartPos, FVector Direction)
{
	CurrentSkillState = ETracerSkillState::BLINK;

	// 전달받은 위치와 방향을 두고 시작
	Owner->SetActorLocation(StartPos);
	BlinkDirection = Direction;
}

void UTracerSkillSystemComponent::ServerRPC_ThrowPulseBomb_Implementation()
{
	if (!bPulseBomb)
		return;

	if (!bPulseBomb)
		return;

	//사용 금지
	bPulseBomb = false;
	//펄스 폭탄이 사용되었으니 쿨타임 갱신 시작
	{
		//아이콘 비활성화 및 게이지 초기화
		ClientRPC_SetSkillIconActivation(PulseBombIconIndex, false);
			
		FTimerHandle cool_TimerHandle;
		//틱 동안 할일은, UI갱신
		auto cool_tick = [&](float cool_deltaTime, float cool_currentTime)
		{
			//쿨타임 게이지 갱신
			ClientRPC_SetSkillCoolTimeUI(PulseBombIconIndex, cool_currentTime, PulseBombCoolTime);
			ClientRPC_SetSkillRemainTimeUI(PulseBombIconIndex, PulseBombCoolTime - cool_currentTime, false);
		};
		//마무리 시 할일은, UI원상복구 후, 사용가능하게 한다.
		auto cool_end = [&](float cool_existTime)
		{
			bPulseBomb = true;
			//아이콘 활성화 및 게이지 초기화
			ClientRPC_SetSkillIconActivation(PulseBombIconIndex, true);
			ClientRPC_SetSkillRemainTimeUI(PulseBombIconIndex, 0, true);
			//스킬 온 사운드 재생
			ClientRPC_PlaySoundSkillOn();
		};

		FDoTimerTick cool_timerDo;
		FNotifyTimerEnd cool_timerEnd;
		cool_timerDo.BindLambda(cool_tick);
		cool_timerEnd.BindLambda(cool_end);

		//만들어둔 쿨타이머 기능을 활용
		CoolTimerManagerComp->RegisterCoolTimerAll(cool_TimerHandle, 0, PulseBombCoolTime, 0.003f, cool_timerDo, cool_timerEnd);
	}
	
	FVector TempStart;
	TempStart = Owner->GetActorLocation() + Owner->GetControlRotation().Vector() * 100;
	APulseBomb* bomb = GetWorld()->SpawnActor<APulseBomb>(PulseBombFactory, TempStart, Owner->GetActorRotation());

	// 일단 앞의 적당한 방향에 던져보는 걸로
	FVector TempDir = Owner->GetActorForwardVector();
	TempDir.Z = TempDir.Z + 1.0f;
	if (bomb)
		bomb->Launch(TempDir, 500.0f, Owner->Controller);
}

void UTracerSkillSystemComponent::ServerRPC_BlinkStart_Implementation(FVector StartPos, FVector Direction)
{
	//=김형모===

	//블링크 카운트가 0이라면 금지한다.
	if (BlinkCount <= 0)
		return;

	//블링크 카운트를 1 줄인다.
	BlinkCount--;
	
	//타이머를 적용한다 (쿨타임 갱신 시작)
	{
		//블링크 아이콘의 블링크 카운트 갱신
		ClientRPC_SetBlinkCountUI(BlinkCount);

		//이번 스킬 사용으로 인해 스킬 카운트가 없어졌다면
		if (BlinkCount <= 0)
		{
			//아이콘 비활성화 및 게이지 초기화
			//강렬하게 금지를 날린다.
			ClientRPC_SetSkillIconActivation(BlinkIconIndex, false, true);
		}

		SetBlinkTimer();
		
		// FTimerHandle cool_TimerHandle;
		// //틱 동안 할일은, UI갱신
		// auto cool_tick = [&](float cool_deltaTime, float cool_currentTime)
		// {
		// 	//쿨타임 게이지 갱신
		// 	//ClientRPC_SetSkillCoolTimeUI(BlinkIconIndex, cool_currentTime, BlinkCoolTime);
		// 	//블링크 아이콘 게이지 갱신
		// 	ClientRPC_SetBlinkIconGage(cool_currentTime, BlinkCoolTime);
		// };
		// //마무리 시 할일은, UI원상복구 후, 사용가능하게 한다.
		// auto cool_end = [&](float cool_existTime)
		// {
		// 	//블링크 카운트를 채워준다.
		// 	BlinkCount++;
		// 	//아이콘 활성화 및 게이지 초기화
		// 	ClientRPC_SetSkillIconActivation(BlinkIconIndex, true);
		// 	//블링크 아이콘 게이지 초기화
		// 	ClientRPC_SetBlinkIconGage(0, 1);
		// 	//블링크 카운트 초기화
		// 	ClientRPC_SetBlinkCountUI(BlinkCount);
		// };
		//
		// FDoTimerTick cool_timerDo;
		// FNotifyTimerEnd cool_timerEnd;
		// cool_timerDo.BindLambda(cool_tick);
		// cool_timerEnd.BindLambda(cool_end);
		//
		// //만들어둔 쿨타이머 기능을 활용
		// CoolTimerManagerComp->RegisterCoolTimerAll(cool_TimerHandle, 0, BlinkCoolTime, 0.003f, cool_timerDo, cool_timerEnd);
	}
	
	//========
	
	// 모두에게 알리기
	MultiRPC_BlinkStart(StartPos, Direction);
	
	// (서버만) 일정 시간 뒤 비활성화 예약
	GetWorld()->GetTimerManager().SetTimer(BlinkTimerHandle, this, &UTracerSkillSystemComponent::DeactivateBlink,
		BlinkDuration,
		false);
}

void UTracerSkillSystemComponent::DebugInfo()
{
	const FString bIsFallingString = Owner->GetCharacterMovement()->IsFalling() ? TEXT("true") : TEXT("false");

	const FString logStr = FString::Printf(
		TEXT("Current Skill State: %s\nCharacterMovement IsFalling: %s"),
		*UEnum::GetValueAsString(CurrentSkillState),
		*bIsFallingString
	);

	DrawDebugString(GetWorld(), Owner->GetActorLocation(), logStr, nullptr, FColor::Red, 0, true, 1);
}