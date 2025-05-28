// Fill out your copyright notice in the Description page of Project Settings.

#include "DuoShooting/Public/Skill/SombraSkillSystemComponent.h"

#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
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
}


// Called when the game starts
void USombraSkillSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	SombraPlayer = Cast<ASombraHero>(TargetPlayer);
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
	UCameraComponent* playerCamera = TargetPlayer->GetCamera();
	ATranslocatorProjectile* newTranslocatorProjectile = GetWorld()->SpawnActor<ATranslocatorProjectile>(OriginTranslocatorProjectile);
	newTranslocatorProjectile->Initializer(this, playerCamera->GetComponentLocation(), playerCamera->GetForwardVector(), ProjectileLaunchSpeed, ProjectileMaxFlyingTime);
}

void USombraSkillSystemComponent::TriggerTranslocator(FVector end)
{
	//이하 scope내 기능은 서버와 클라이언트에서, 본인인지 여부에 따라 처리가 달라질 수 있다.
	{
		SombraPlayer->SetDisAppearance();
	}
	
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
		//이하 scope내 기능은 서버와 클라이언트에서, 본인인지 여부에 따라 처리가 달라질 수 있다.
		{
			SombraPlayer->SetAppearance();
		}
		
		UE_LOG(LogTemp, Error, TEXT("End"));
		TargetPlayer->SetActorLocation(end);

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