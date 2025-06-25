// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack/HitscanEmitterComponent.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeSourceComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Management/TeamFightPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/HeroBase.h"
#include "Particles/ParticleSystem.h"


// Sets default values for this component's properties
UHitscanEmitterComponent::UHitscanEmitterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Owner = nullptr;
	OwnerCamera = nullptr;

	{
		ConstructorHelpers::FObjectFinder<UInputAction> TempIA(
			TEXT("'/Game/DuoShooting/Inputs/HeroDefaults/IA_HeroFire.IA_HeroFire'"));
		if (TempIA.Succeeded()) { IA_Fire = TempIA.Object; }
	}
	{
		ConstructorHelpers::FObjectFinder<UInputAction> TempIA(
			TEXT("'/Game/DuoShooting/Inputs/HeroDefaults/IA_HeroReload.IA_HeroReload'"));
		if (TempIA.Succeeded()) { IA_Reload = TempIA.Object; }
	}
	{
		// HeroBase용 임시.. 각자 캐릭터에서 새로 지정할 것
		ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(
			TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_Explosion1.P_Explosion1'"));
		//'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
		if (ParticleAsset.Succeeded()) { FireParticle = ParticleAsset.Object; }
	}

	SetIsReplicated(true);

	//==김형모
	//사운드 관련 데이터 가져오기
	{
		//발사 관련 사운드
		ConstructorHelpers::FObjectFinder<USoundBase> soundShoot(TEXT(
			"/Script/Engine.SoundWave'/Game/ProjectSD/Blueprints/Character/Player/Player_Shot_AR.Player_Shot_AR'"));
		if (soundShoot.Succeeded())
		{
			OriginSoundShoot = soundShoot.Object;
		}
		ConstructorHelpers::FObjectFinder<USoundAttenuation> soundAttenuation(TEXT(
			"/Script/Engine.SoundAttenuation'/Game/DuoShooting/Sounds/Sombra/SA_StealthEnterExit.SA_StealthEnterExit'"));
		if (soundAttenuation.Succeeded())
		{
			OriginSoundAttenuation = soundAttenuation.Object;
		}
		//장전 사운드
		ConstructorHelpers::FObjectFinder<USoundBase> soundReload(TEXT(
			"/Script/Engine.SoundWave'/Game/Resources/Sounds/Weapon/AR_1005/RW_AR_1005_Reload.RW_AR_1005_Reload'"));
		if (soundReload.Succeeded())
		{
			OriginSoundReload = soundReload.Object;
		}
	}
}

// Called when the game starts
void UHitscanEmitterComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<AHeroBase>(GetOwner());
	if (Owner) { OwnerCamera = Owner->GetCamera(); }
	else UE_LOG(LogTemp, Error, TEXT("UHitscanComponent이 AHeroBase 주인을 찾지 못하고 카메라도 가져오지 못함"));
}

// Called every frame
void UHitscanEmitterComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickHitScan(DeltaTime);
}

void UHitscanEmitterComponent::SetState(EHitscanEmitterState newState)
{
	if (newState != State)
	{
		// EXIT
		switch (State)
		{
			// 트리거 상태에서 나갈 때
		case EHitscanEmitterState::TRIGGERED:
			FireTimer = 1000.0f; // 충분히 큰 수로 타이머를 초기화 시켜준다
			break;
		default:
			break;
		}
		
		State = newState;
	}
}

void UHitscanEmitterComponent::SetCurrentBullet(int32 bullets)
{
	CurrentBullet = bullets;

	if (ShootingMainWidget) ShootingMainWidget->SetCurrentBullet(CurrentBullet);
}

void UHitscanEmitterComponent::SingleLineTrace()
{
	FHitResult Result;

	// 카메라에서 시작
	FVector Start = OwnerCamera->GetComponentLocation();

	// 카메라 앞방향에서 설정한만큼 흐트러지게 설정
	FVector ForwardVector = Owner->GetControlRotation().Vector();
	if (Spread > 0.0f)
	{
		float MaxAngleInDegrees = Spread;

		FVector RandomSpread = FMath::VRandCone(ForwardVector, FMath::DegreesToRadians(MaxAngleInDegrees));
		ForwardVector = RandomSpread;
	}

	FVector End = Start + ForwardVector * MaxDistance;

	// 스스로는 무시
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	bool bHit = GetWorld()->LineTraceSingleByChannel(Result, Start, End, ECC_Visibility, Params);

	if (bHit)
	{
		//// 테스트용 시각화
		//DrawDebugLine(GetWorld(), Start, Result.Location, FColor::Green, false, 5.0f);

		// 히어로가 맞았으면 피해 주기
		if (auto* hero = Cast<AHeroBase>(Result.GetActor()))
		{
			UGameplayStatics::ApplyDamage(hero, DamagePerBullet, Owner->GetController(), Owner,
			                              UDamageType::StaticClass());

			// 다른 팀을 맞췄을 때만 이펙트 재생
			if (auto* instigatorPlayerState = Cast<ATeamFightPlayerState>(Owner->GetController()->PlayerState))
			{
				if (auto* hitPlayerState = Cast<ATeamFightPlayerState>(hero->GetController()->PlayerState))
				{
					if (instigatorPlayerState->GetPlayerTeam() != hitPlayerState->GetPlayerTeam())
					{
						MultiRPC_ReceiveSingleLineTraceResult(Result.Location);
					}
				}
			}
		}
		// 히어로가 아닌 일반 사물이 맞았을 경우
		else
		{
			MultiRPC_ReceiveSingleLineTraceResult(Result.Location);
		}
	}
}

void UHitscanEmitterComponent::StartReload()
{
	// 이미 리로딩 중이면 리턴
	if (State == EHitscanEmitterState::RELOADING || State == EHitscanEmitterState::BLOCKED) return;
	SetState(EHitscanEmitterState::RELOADING);

	ServerRPC_Reload();
}

void UHitscanEmitterComponent::EndReload()
{
	// 총알 채우기
	SetCurrentBullet(Owner->GetMaxBullet());

	// 타이머 초기화
	FireTimer = 1000.0f; // 충분히 큰 수

	UE_LOG(LogTemp, Warning, TEXT("재장전 완료"));
}

// 로컬 컨트롤러가 발사 시점을 결정하고, 서버 -> 모두에게 알림
void UHitscanEmitterComponent::TickHitScan(float dt)
{
	if (Owner == nullptr) return;
	if (!Owner->IsLocallyControlled()) return;
	if (CurrentBullet <= 0) return;
	if (State != EHitscanEmitterState::TRIGGERED) return;

	// 트리거되어있다면 연사
	FireTimer += dt;
	if (FireTimer >= FireInterval)
	{
		// 타이머 초기화
		FireTimer = 0.0f;

		// 서버면 직접 Implementation 실행, 아니면 서버 RPC로 총 쏘기 리퀘스트
		if (Owner->HasAuthority()) ServerRPC_RequestSingleLineTrace_Implementation();
		else ServerRPC_RequestSingleLineTrace();

		// Owner에게만 보일 시각 효과들
		// 카메라 쉐이크
		if (CameraShakeSourceComp && FireCameraShake)
		{
			//CameraShakeSourceComp->StartCameraShake(FireCameraShake, 1.0f);
		}
	}
}

void UHitscanEmitterComponent::SetupHitscanInputInfo(UEnhancedInputComponent* enhancedInputComponent)
{
	enhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Started, this,
	                                   &UHitscanEmitterComponent::InputFire_Started);
	enhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Completed, this,
	                                   &UHitscanEmitterComponent::InputFire_Completed);
	enhancedInputComponent->BindAction(IA_Reload, ETriggerEvent::Triggered, this,
	                                   &UHitscanEmitterComponent::InputReload);
}

void UHitscanEmitterComponent::Initialize(UShootingMainWidget* mainWidgetInst,
                                          UCameraShakeSourceComponent* camShakeSourceInst)
{
	ShootingMainWidget = mainWidgetInst;
	CameraShakeSourceComp = camShakeSourceInst;

	SetCurrentBullet(Owner->GetMaxBullet());
}

void UHitscanEmitterComponent::InputFire_Started()
{
	// 기본 상태에서만 총 쏘기 가능
	if (State != EHitscanEmitterState::IDLE) return;
	SetState(EHitscanEmitterState::TRIGGERED);

	ServerRPC_InputFireStarted();
	
	Owner->ServerRPC_DoAfterAction(EHeroActionType::NormalAttackStart);
}

void UHitscanEmitterComponent::InputFire_Completed()
{
	// 트리거되어있을 때만 실행
	if (State != EHitscanEmitterState::TRIGGERED) return;
	SetState(EHitscanEmitterState::IDLE);

	ServerRPC_InputFireCompleted();
	Owner->ServerRPC_DoAfterAction(EHeroActionType::NormalAttackEnd);
}

void UHitscanEmitterComponent::SetHitScanSettings(float fireInterval, float damagePerBullet, float spread,
	float maxDist)
{
	FireInterval = fireInterval;
	DamagePerBullet = damagePerBullet;
	Spread = spread;
	MaxDistance = maxDist;
}

void UHitscanEmitterComponent::InputReload()
{
	// 총알이 이미 풀이면 리턴
	if (CurrentBullet >= Owner->GetMaxBullet()) return;

	// 이미 리로딩 중이면 리턴
	if (State == EHitscanEmitterState::RELOADING) return;

	// 히트스킨의 인풋 자체가 막혀있어도 리턴
	if (State == EHitscanEmitterState::BLOCKED) return;

	SetState(EHitscanEmitterState::RELOADING);

	ServerRPC_Reload();
}

void UHitscanEmitterComponent::Enable()
{
	if (State == EHitscanEmitterState::BLOCKED)
	{
		SetState(EHitscanEmitterState::IDLE);
		ServerRPC_Enable();
	}
}

void UHitscanEmitterComponent::Disable()
{
	if (State != EHitscanEmitterState::BLOCKED)
	{
		SetState(EHitscanEmitterState::BLOCKED);
		ServerRPC_Disable();
	}
}

void UHitscanEmitterComponent::ServerRPC_RequestSingleLineTrace_Implementation()
{
	// 총알 쓰기
	SetCurrentBullet(CurrentBullet - 1);
	ClientRPC_ReceiveSingleLineTraceResult(CurrentBullet);

	// 라인트레이스 쏘기
	SingleLineTrace();

	//총기 사운드 발생 요청
	MultiRPC_PlaySoundShoot();

	Owner->ServerRPC_DoAfterAction_Implementation(EHeroActionType::NormalAttackSuccess);
}

void UHitscanEmitterComponent::ServerRPC_InputFireCompleted_Implementation()
{
	SetState(EHitscanEmitterState::IDLE);
}

void UHitscanEmitterComponent::MultiRPC_ReceiveSingleLineTraceResult_Implementation(FVector hitLocation)
{
	// 이펙트
	if (FireParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireParticle, hitLocation);
	}
}

void UHitscanEmitterComponent::ClientRPC_ReceiveSingleLineTraceResult_Implementation(int bulletCount)
{
	// 총알 개수 적용
	SetCurrentBullet(bulletCount);

	// 총알이 다 닳으면 자동 재장전
	if (CurrentBullet <= 0) InputReload();
}

void UHitscanEmitterComponent::ServerRPC_InputFireStarted_Implementation()
{
	SetState(EHitscanEmitterState::TRIGGERED);
}

void UHitscanEmitterComponent::ServerRPC_Reload_Implementation()
{
	SetState(EHitscanEmitterState::RELOADING);

	// 1초후에 리로딩 완료 예약
	FTimerHandle TempReloadHandle;
	GetWorld()->GetTimerManager().SetTimer(TempReloadHandle, this, &UHitscanEmitterComponent::Server_EndReloading, 1.0f,
	                                       false);

	// 총기 장전 사운드 시작 요청
	ClientRPC_PlaySoundReload();
	
	// 장전 시작 시점 트리거
	Owner->ServerRPC_DoAfterAction(EHeroActionType::ReloadStart);
}

void UHitscanEmitterComponent::ClientRPC_ReloadEnd_Implementation(int bulletCount)
{
	// 총알 개수 풀로 돌려두기
	SetCurrentBullet(bulletCount);

	// 원래 상태로 돌려놓기
	SetState(EHitscanEmitterState::IDLE);
}

void UHitscanEmitterComponent::Server_EndReloading()
{
	// 원래 상태로 돌려놓기
	SetState(EHitscanEmitterState::IDLE);

	// 총알 채우기
	SetCurrentBullet(Owner->GetMaxBullet());

	ClientRPC_ReloadEnd(CurrentBullet);
}

void UHitscanEmitterComponent::ServerRPC_Enable_Implementation()
{
	SetState(EHitscanEmitterState::IDLE);
}

void UHitscanEmitterComponent::ServerRPC_Disable_Implementation()
{
	SetState(EHitscanEmitterState::BLOCKED);
}

void UHitscanEmitterComponent::MultiRPC_PlaySoundShoot_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), OriginSoundShoot, Owner->GetActorLocation(), 1, 1, 0,
	                                      OriginSoundAttenuation);
}

void UHitscanEmitterComponent::ClientRPC_PlaySoundReload_Implementation()
{
	UGameplayStatics::PlaySound2D(GetWorld(), OriginSoundReload, 1, 1, 0);
}
