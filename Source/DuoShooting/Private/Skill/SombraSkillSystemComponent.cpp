// Fill out your copyright notice in the Description page of Project Settings.

#include "DuoShooting/Public/Skill/SombraSkillSystemComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/HeroBase.h"
#include "Player/SombraHero.h"
#include "Skill/SombraSkill/TranslocatorProjectile.h"
#include "Tool/CoolTimerManagerComponent.h"

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
	//HackOff
	ConstructorHelpers::FObjectFinder<UInputAction> ia_hackOff(TEXT("/Script/EnhancedInput.InputAction'/Game/DuoShooting/Inputs/Sombra/IA_SombraHackOff.IA_SombraHackOff'"));
	if (ia_emp.Succeeded())
	{
		IA_Hack = ia_hackOff.Object;
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

	//발사체 원본 불러오기
	ConstructorHelpers::FClassFinder<ATranslocatorProjectile> translocatorProjectile(TEXT("/Script/Engine.Blueprint'/Game/DuoShooting/Blueprints/Characters/Skill/Sombra/BP_TranslocatorProjectile.BP_TranslocatorProjectile_C'"));
	if (translocatorProjectile.Succeeded())
	{
		OriginTranslocatorProjectile = translocatorProjectile.Class;
	}

	//솜브라 투사체 스킬 아이콘 텍스쳐 불러오기
	ConstructorHelpers::FObjectFinder<UTexture2D> translocatorTexture2D(TEXT("/Script/Engine.Texture2D'/Game/DuoShooting/Sprites/SkillIcons/SombraTranslocator.SombraTranslocator'"));
	if (translocatorTexture2D.Succeeded())
	{
		OriginTranslocatorTexture2D = translocatorTexture2D.Object;
	}
}


// Called when the game starts
void USombraSkillSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// ...
	SombraPlayer = Cast<ASombraHero>(TargetPlayer);
	CalHackConeTrace();


#if WITH_EDITOR
	
		// FTimerHandle emptyHandle;
		// GetWorld()->GetTimerManager().SetTimer(emptyHandle, [&]()->void
		// {
		// 	//if (nullptr == Cast<APlayerController>(SombraPlayer->Controller))
		// 	if (ROLE_Authority != SombraPlayer->GetRemoteRole() && ROLE_Authority != SombraPlayer->GetLocalRole()) 
		// 	{
		// 		SombraPlayer->SetActorRotation(SombraPlayer->GetActorRotation() + FRotator(0, 90, 0));
		// 		FInputActionValue emptyValue = FInputActionValue();
		// 		OnTranslocator(emptyValue);
		// 	}
		// }, 10.f, true);
#endif

	if (TargetPlayer->IsLocallyControlled())
		TranslocatorIconIndex = AddSkillUI(OriginTranslocatorTexture2D, FText::FromString(TEXT("E")));
	else
	{
		//서버용 동기화 임시
		TranslocatorIconIndex = 0;
	}
}


// Called every frame
void USombraSkillSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	HackTick(DeltaTime);
}

void USombraSkillSystemComponent::SetupHeroInputInfo(class UEnhancedInputComponent* enhancedInputComponent)
{
	enhancedInputComponent->BindAction(IA_EMP, ETriggerEvent::Started, this, &USombraSkillSystemComponent::OnEMP);
	enhancedInputComponent->BindAction(IA_Hack, ETriggerEvent::Triggered, this, &USombraSkillSystemComponent::OnHack);
	enhancedInputComponent->BindAction(IA_HackOff, ETriggerEvent::Completed, this, &USombraSkillSystemComponent::OnHackCancled);
	enhancedInputComponent->BindAction(IA_Virus, ETriggerEvent::Started, this, &USombraSkillSystemComponent::OnVirus);
	enhancedInputComponent->BindAction(IA_Translocator, ETriggerEvent::Started, this, &USombraSkillSystemComponent::OnTranslocator);
	TargetPlayerEnhancedInputComponent = enhancedInputComponent;
	UE_LOG(LogTemp, Error, TEXT("setupinput"));
}

void USombraSkillSystemComponent::OnEMP(const struct FInputActionValue& value)
{
	
}

void USombraSkillSystemComponent::OnHack(const struct FInputActionValue& value)
{
	//쿨타임 적용도 필요.
	
	if (nullptr == HackTarget)
	{
		BeforeHackTarget = nullptr;
		return;
	}

	if (BeforeHackTarget != HackTarget)
	{
		CurrentHackTryTime = 0;
	}

	BeforeHackTarget = HackTarget;

	CurrentHackTryTime += GetWorld()->GetDeltaSeconds();
	if (CurrentHackTryTime >= MaxHackTryTime)
	{
		UE_LOG(LogTemp, Error, TEXT("S"));
		//실제 해킹을 걸었을 때 해야 할 일.
		DrawDebugLine(GetWorld(), HackTarget->GetActorLocation(), HackTarget->GetActorLocation() + FVector::UpVector * 1000.f, FColor::Yellow, false, 1.f);
		FTimerHandle timerHandle;
		if (nullptr != Cast<APlayerController>(HackTarget->Controller))
		{
			HackTarget->GetSkillSystemComponent()->RemoveHeroInputInfo();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("not player remove input"));
		}
		GetWorld()->GetTimerManager().SetTimer(timerHandle, [&]()->void
		{
			if (nullptr == Cast<APlayerController>(HackTarget->Controller))
			{
				UE_LOG(LogTemp, Error, TEXT("not player resest input"));
				return;
			}
			HackTarget->GetSkillSystemComponent()->ReSetupHeroInputInfo();
		}, 1.0f, false);
		CurrentHackTryTime = 0;
	}
}

void USombraSkillSystemComponent::OnHackCancled(const struct FInputActionValue& value)
{
	CurrentHackTryTime = 0;
}

void USombraSkillSystemComponent::OnVirus(const struct FInputActionValue& value)
{
}

void USombraSkillSystemComponent::OnTranslocator(const struct FInputActionValue& value)
{
	ServerRPC_OnTranslocator(value);
}

void USombraSkillSystemComponent::ServerRPC_OnTranslocator_Implementation(const struct FInputActionValue& value)
{
	if (!bTranslocator)
		return;

	//스킬 사용 제한 및 쿨타임 재기
	{
		//사용 중지
		bTranslocator = false;

		//나머지는 실제 쿨타임 시작 지점에서 시작.
		//위치변환기는 순간이동 직후임. TriggerTranslocator 함수 쪽으로 가보면 됨
	}

	
	UCameraComponent* playerCamera = TargetPlayer->GetCamera();
	ATranslocatorProjectile* newTranslocatorProjectile = GetWorld()->SpawnActor<ATranslocatorProjectile>(OriginTranslocatorProjectile, playerCamera->GetComponentLocation(), TargetPlayer->GetControlRotation());
	newTranslocatorProjectile->Initializer(this, playerCamera->GetComponentLocation(), TargetPlayer->GetControlRotation().Vector(), ProjectileLaunchSpeed, ProjectileMaxFlyingTime);
}

void USombraSkillSystemComponent::TakeDamage()
{
	Super::TakeDamage();

	if (HitDetectionTimerHandle.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(HitDetectionTimerHandle);

	SetDetectionLayer(EDetection::HitDetection, true);
	GetWorld()->GetTimerManager().SetTimer(HitDetectionTimerHandle, [&]()
	{
		SetDetectionLayer(EDetection::HitDetection, false);		
	}, HitDetectionTime, false);
}

void USombraSkillSystemComponent::TriggerTranslocator(FVector end)
{
	SombraPlayer->MultiRPC_SetAppearanceTP(false);
	
	FVector start = TargetPlayer->GetActorLocation();
	//이동 시 무적으로 할 거라 임시변수로 괜찮음. 중간에 끊을 일이 없을 것으로 판단
	FTimerHandle timerHandle;

	//람다식으로 넘길 거라 매개체가 되어줄 델리게이트 변수
	FDoTimerTick doTimerTick;
	FNotifyTimerEnd notifyTimerEnd;
	
	auto TickTranslocator = [&, start, end](float deltaTime, float currentTime)->void
	{
		FVector nextEnd = FMath::Lerp(start, end, currentTime / MoveTime);
		TargetPlayer->SetActorLocation(nextEnd);
	};
	
	auto EndTranslocator = [&, end](float deltaTime)->void
	{
		SombraPlayer->MultiRPC_SetAppearanceTP(true);
		
		UE_LOG(LogTemp, Error, TEXT("End"));
		TargetPlayer->SetActorLocation(end);
		
		//순간이동이 종료되었으니 쿨타임 갱신 시작
		{
			//아이콘 비활성화 및 게이지 초기화
			ClientRPC_SetSkillIconActivation(TranslocatorIconIndex, false);
			
			FTimerHandle cool_TimerHandle;
			//틱 동안 할일은, UI갱신
			auto cool_tick = [&](float cool_deltaTime, float cool_currentTime)
			{
				//쿨타임 게이지 갱신
				ClientRPC_SetSkillCoolTimeUI(TranslocatorIconIndex, cool_currentTime, TranslocatorCoolTime);
			};
			//마무리 시 할일은, UI원상복구 후, 사용가능하게 한다.
			auto cool_end = [&](float cool_existTime)
			{
				bTranslocator = true;
				//아이콘 활성화 및 게이지 초기화
				ClientRPC_SetSkillIconActivation(TranslocatorIconIndex, true);
			};

			FDoTimerTick cool_timerDo;
			FNotifyTimerEnd cool_timerEnd;
			cool_timerDo.BindLambda(cool_tick);
			cool_timerEnd.BindLambda(cool_end);

			//만들어둔 쿨타이머 기능을 활용
			CoolTimerManagerComp->RegisterCoolTimerAll(cool_TimerHandle, 0, TranslocatorCoolTime, 0.003f, cool_timerDo, cool_timerEnd);
		}
		
		StartStealth();
	};

	doTimerTick.BindLambda(TickTranslocator);
	notifyTimerEnd.BindLambda(EndTranslocator);
	
	CoolTimerManagerComp->RegisterCoolTimerAll(timerHandle, 0.f, MoveTime, 0.0003f, doTimerTick, notifyTimerEnd);
}

void USombraSkillSystemComponent::SetDetectionLayer(EDetection newDetection, bool bSwitch)
{
	//은신 상태가 아니라면 값도 바꾸지 말아라.
	if (!CoolTimer_StealthTimerHandle.IsValid())
		return;

	//0이라면 전 상태는 false, 0보다 크다면 전 상태는 true
	bool beforeState = DetectionLayer > 0;
	
	//bitmask 준비
	uint32 bitmask = 1;
	//enum값에 맞게 bitmask 값 설정
	bitmask <<= static_cast<uint32>(newDetection);
	
	if (bSwitch) //스위치 온
	{
		//현재 layer에 적용
		DetectionLayer |= bitmask;
	}
	else //스위치 오프
	{
		//기존 값이 true였는지 false였는지 판단
		uint32 bitmaskCheck = DetectionLayer & bitmask;
		//true였다면 0으로 바꾸고
		if (bitmaskCheck == bitmask)
		{
			DetectionLayer -= bitmask;
		}
		//false였다면 그대로 내보낸다.
	}

	//0이라면 후 상태는 false, 0보다 크다면 후 상태는 true
	bool afterState = DetectionLayer > 0;

	//만약 DetactionLayer가 있음에도 현재 StealthState가 Detecion이 아니라면, Detection으로 전환하는 작업 추가.
	//밑의 로직 특성상, 중간에 누가 상태를 가로채면, 복구를 할 수 없어서 그럼.
	if (true == afterState && EStealthState::Detection != SombraPlayer->GetStealthState())
	{
		SombraPlayer->SetStealthState(EStealthState::Detection);
		return;
	}
	
	//상태가 없었다가 추가되었는지, 마지막으로 빠져나가는 건지를 판단하기 위한 비교문.
	//값이 /0->1이상/으로 바뀔 때, 은신 세부 상태를 Detection으로 바꾼다.
	if (beforeState == false && afterState == true)
	{
		SombraPlayer->SetStealthState(EStealthState::Detection);
	}
	//값이 /1이상->0/으로 바뀔 때, 은신 세부 상태를 Hidden으로 바꾼다.
	else if (beforeState == true && afterState == false)
	{
		SombraPlayer->SetStealthState(EStealthState::Hidden);
	}
}

void USombraSkillSystemComponent::StartStealth()
{
	SombraPlayer->EnterStealth();
	CoolTimerManagerComp->RegisterCoolTimerAll(this, CoolTimer_StealthTimerHandle, 0, StealthTime, 0.0003f, &USombraSkillSystemComponent::StealthTick, &USombraSkillSystemComponent::NotifyStealthEnd);
}

void USombraSkillSystemComponent::EndStealth()
{
	DetectionLayer = 0;
	if (CoolTimer_StealthTimerHandle.IsValid())
		CoolTimerManagerComp->RemoveTimer(CoolTimer_StealthTimerHandle);
	SombraPlayer->ExitStealth();
}

void USombraSkillSystemComponent::StealthTick(float deltaTime, float currentTime)
{
	
	//주변 적 감지
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	UClass* ActorClassFilter = AHeroBase::StaticClass();
	TArray<AActor*> ActorsToIgnore;
	//자신은 감지 제외
	ActorsToIgnore.Add(SombraPlayer);
	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), SombraPlayer->GetActorLocation(), DetectionRadius, ObjectTypes, ActorClassFilter, ActorsToIgnore,OutActors);
	if (OutActors.Num() > 0) //감지되면 mask on
	{
		SetDetectionLayer(EDetection::PlayerDetection, true);
	}
	else //감지되면 mask off
	{
		SetDetectionLayer(EDetection::PlayerDetection, false);
	}
}

void USombraSkillSystemComponent::NotifyStealthEnd(float excessDeltaTime)
{
	EndStealth();
}

bool USombraSkillSystemComponent::DetectHackTarget(class AHeroBase*& outHeroBase)
{
	CalSecondHackTraceBaseStartPoint();
	AHeroBase* firstHero = nullptr;
	AHeroBase* secondHero = nullptr;
	DetectHackTargetInFirstHackTrace(firstHero);
	DetectHackTargetInSecondHackTrace(secondHero);
	
	if(nullptr == firstHero && nullptr == secondHero)
	{
		outHeroBase = nullptr;
		return false;
	}
	if (nullptr == firstHero)
	{
		outHeroBase = secondHero;
		return true;
	}
	if (nullptr == secondHero)
	{
		outHeroBase = firstHero;
		return true;
	}
	
	float firstDistance = FVector::Dist(firstHero->GetActorLocation(), TargetPlayer->GetCamera()->GetComponentLocation());
	float secondDistance = FVector::Dist(firstHero->GetActorLocation(), TargetPlayer->GetCamera()->GetComponentLocation());
	
	if (firstDistance <= secondDistance)
	{
		outHeroBase = firstHero;
		return true;
	}
	else
	{
		outHeroBase = secondHero;
		return true;
	}
}

void USombraSkillSystemComponent::CalHackConeTrace()
{
	HackTargetCalInfo.ra = HackTargetCalInfo.da * UKismetMathLibrary::DegTan(HackTargetCalInfo.alpha / 2);
	HackTargetCalInfo.dbp = HackTargetCalInfo.ra / UKismetMathLibrary::DegTan(HackTargetCalInfo.beta / 2) - HackTargetCalInfo.da;
	HackTargetCalInfo.rb = UKismetMathLibrary::DegTan(HackTargetCalInfo.beta / 2) * (HackTargetCalInfo.db + HackTargetCalInfo.dbp);
}

void USombraSkillSystemComponent::CalSecondHackTraceBaseStartPoint()
{
	UCameraComponent* targetCamera = TargetPlayer->GetCamera();
	HackTargetCalInfo.SecondHackStartPoint = targetCamera->GetComponentLocation() + targetCamera->GetForwardVector() * -1 * HackTargetCalInfo.dbp; 
}

bool USombraSkillSystemComponent::DetectHackTargetInFirstHackTrace(AHeroBase*& outHeroBase)
{
	//감지 기반 데이터 계산
	UCameraComponent* targetCamera = TargetPlayer->GetCamera();
	//방향
	FVector detectPoint = targetCamera->GetComponentLocation() + targetCamera->GetForwardVector() * HackTargetCalInfo.da / 2;
	float halfSizeX = HackTargetCalInfo.da / 2;
	float halfSizeY = HackTargetCalInfo.ra;
	float halfSizeZ = HackTargetCalInfo.ra;
	FRotator detectOrientation = UKismetMathLibrary::MakeRotFromXZ(targetCamera->GetForwardVector(), targetCamera->GetUpVector());

	//
	TArray<AActor*> outActors;
	
	//충돌 감지
	TArray<FHitResult> hitResults;
	TArray<AActor*> ignoreActors;
	ignoreActors.Add(TargetPlayer);
	if (!UKismetSystemLibrary::BoxTraceMulti(GetWorld(), detectPoint, detectPoint, FVector(halfSizeX, halfSizeY, halfSizeZ), detectOrientation,\
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2), false, ignoreActors, EDrawDebugTrace::ForOneFrame, hitResults, true, FLinearColor::Red, FLinearColor::Green), -1.f)
		return false;

	AHeroBase* hitHero = nullptr;
	for (auto hitResult : hitResults)
	{
		hitHero = Cast<AHeroBase>(hitResult.GetActor()); 
		if (hitHero == nullptr)
			continue;
		
		FVector actorLocation = hitResult.GetActor()->GetActorLocation();
		float coneDegree = UKismetMathLibrary::DegAcos(FVector::DotProduct((actorLocation - targetCamera->GetComponentLocation()).GetSafeNormal(), targetCamera->GetForwardVector()));
		if (coneDegree <= (HackTargetCalInfo.alpha / 2))
		{
			outActors.Add(hitResult.GetActor());
		}
	}

	float distance = -1.f;
	outHeroBase = Cast<AHeroBase>(UGameplayStatics::FindNearestActor(targetCamera->GetComponentLocation(), outActors, distance));

	if (outHeroBase == nullptr)
		return false;
	return true;
}

bool USombraSkillSystemComponent::DetectHackTargetInSecondHackTrace(AHeroBase*& outHeroBase)
{
	//감지 기반 데이터 계산
	UCameraComponent* targetCamera = TargetPlayer->GetCamera();
	//방향
	FVector detectPoint = targetCamera->GetComponentLocation() + targetCamera->GetForwardVector() * (HackTargetCalInfo.da + (HackTargetCalInfo.db - HackTargetCalInfo.da) / 2);
	float halfSizeX = (HackTargetCalInfo.db - HackTargetCalInfo.da) / 2;
	float halfSizeY = HackTargetCalInfo.rb;
	float halfSizeZ = HackTargetCalInfo.rb;
	FRotator detectOrientation = UKismetMathLibrary::MakeRotFromXZ(targetCamera->GetForwardVector(), targetCamera->GetUpVector());

	//
	TArray<AActor*> outActors;
	
	//충돌 감지
	TArray<FHitResult> hitResults;
	TArray<AActor*> ignoreActors;
	ignoreActors.Add(TargetPlayer);
	if (!UKismetSystemLibrary::BoxTraceMulti(GetWorld(), detectPoint, detectPoint, FVector(halfSizeX, halfSizeY, halfSizeZ), detectOrientation,\
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2), false, ignoreActors, EDrawDebugTrace::ForOneFrame, hitResults, true, FLinearColor::Blue, FLinearColor::Black), -1.f)
		return false;

	AHeroBase* hitHero = nullptr;
	for (auto hitResult : hitResults)
	{
		hitHero = Cast<AHeroBase>(hitResult.GetActor()); 
		if (hitHero == nullptr)
			continue;
		
		FVector actorLocation = hitResult.GetActor()->GetActorLocation();
		float coneDegree = UKismetMathLibrary::DegAcos(FVector::DotProduct((actorLocation - HackTargetCalInfo.SecondHackStartPoint).GetSafeNormal(), targetCamera->GetForwardVector()));
		if (coneDegree <= (HackTargetCalInfo.beta / 2))
		{
			outActors.Add(hitResult.GetActor());
		}
	}

	float distance = -1.f;
	outHeroBase = Cast<AHeroBase>(UGameplayStatics::FindNearestActor(targetCamera->GetComponentLocation(), outActors, distance));

	if (outHeroBase == nullptr)
		return false;
	return true;
}

void USombraSkillSystemComponent::HackTick(float deltaTime)
{
	if (TargetPlayer == nullptr)
		return;

	/*
	if (DetectHackTarget(HackTarget))
	{
	}
	*/
}

// //Test======================
//
//
// void USombraSkillSystemComponent::BTick(float deltaTimes, float currentTimes)
// {
// 	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
// 	UClass* ActorClassFilter = AHeroBase::StaticClass();
// 	TArray<AActor*> ActorsToIgnore;
// 	TArray<AActor*> OutActors;
// 	
// 	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), SombraPlayer->GetActorLocation(), 1000.f, ObjectTypes, ActorClassFilter, ActorsToIgnore,OutActors);
//
// 	if (OutActors.Num() > 0 && SombraPlayer->)
// 	{
// 		
// 	}
// 	
// }