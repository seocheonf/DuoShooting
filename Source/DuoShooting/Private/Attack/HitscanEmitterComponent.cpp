// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack/HitscanEmitterComponent.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeSourceComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
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
			TEXT("'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
		if (ParticleAsset.Succeeded()) { FireParticle = ParticleAsset.Object; }
	}
}

void UHitscanEmitterComponent::SetHitScanSettings(float fireInterval, float damagePerBullet, float spread,
	float maxDist)
{
	FireInterval = fireInterval;
	DamagePerBullet = damagePerBullet;
	Spread = spread;
	MaxDistance = maxDist;
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
	//DebugInfo();
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
			//// 전달할 FPointDamageEvent 구조체 구성
			//FPointDamageEvent damageEvent;
			//damageEvent.Damage = DamagePerBullet; // 얘는 빌트인 TakeDamage 내부에서 안쓰이긴 하는데 일단은 넣어두자
			//damageEvent.HitInfo = Result;

			// 피해 주기
			//hero->TakeDamage(DamageAmount, damageEvent, Owner->GetController(), Owner);
			UGameplayStatics::ApplyDamage(hero, DamagePerBullet, Owner->GetController(), Owner,
				UDamageType::StaticClass());
		}

		Owner->MultiRPC_FireEffects(Result.Location);
	}
}

void UHitscanEmitterComponent::StartReload()
{
	// 이미 리로딩 중이면 리턴
	if (bReloading) return;

	bReloading = true;

	Owner->ServerRPC_Reload();
}

void UHitscanEmitterComponent::EndReload()
{
	// 총알 채우기
	SetCurrentBullet(Owner->GetMaxBullet());

	// 타이머 초기화
	FireTimer = 1000.0f; // 충분히 큰 수

	UE_LOG(LogTemp, Warning, TEXT("재장전 완료"));
}

void UHitscanEmitterComponent::TickHitScan(float dt)
{
	// 서버에서만 돌아가는 코드
	if (!Owner->HasAuthority()) return;
	if (!bEnabled) return;
	if (!bTriggered) return;
	if (bReloading) return;
	if (CurrentBullet <= 0) return;

	// 트리거되어있다면 연사
	FireTimer += dt;
	if (FireTimer >= FireInterval)
	{
		// 타이머 초기화
		FireTimer = 0.0f;

		// 총알 쓰기
		SetCurrentBullet(CurrentBullet - 1);
		Owner->ClientRPC_FireHitScan(CurrentBullet);

		// 라인트레이스 쏘기
		SingleLineTrace();

		//UE_LOG(LogTemp, Warning, TEXT("남은 총알: %d"), CurrentBullet);
	}
}

void UHitscanEmitterComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(UHitscanEmitterComponent, bEnabled);
	//DOREPLIFETIME(UHitscanEmitterComponent, bTriggered);
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
	// 이미 트리거되어 있다면 리턴
	if (bTriggered) return;
	bTriggered = true;
	UE_LOG(LogTemp, Warning, TEXT("InputFire_Started"));
	
	Owner->ServerRPC_FireHitScan(true);

	Owner->DoAfterAction(EHeroActionType::NormalAttackStart);
}

void UHitscanEmitterComponent::InputFire_Completed()
{
	// 이미 트리거되어있지 않다면 리턴
	if (!bTriggered) return;
	bTriggered = false;
	FireTimer = 1000.0f; // 충분히 큰 수
	UE_LOG(LogTemp, Warning, TEXT("InputFire_Completed"));

	Owner->ServerRPC_FireHitScan(false);

	Owner->DoAfterAction(EHeroActionType::NormalAttackEnd);
}

void UHitscanEmitterComponent::InputReload()
{
	if (CurrentBullet >= Owner->GetMaxBullet()) return;

	bReloading = true;
	Owner->ServerRPC_Reload();
}

void UHitscanEmitterComponent::Enable()
{
	if (bEnabled) return;
	bEnabled = true;
	UE_LOG(LogTemp, Warning, TEXT("총 활성화"));
}

void UHitscanEmitterComponent::Disable()
{
	if (!bEnabled) return;
	bEnabled = false;
	UE_LOG(LogTemp, Warning, TEXT("총 비활성화"));
}

// 서버용 함수
void UHitscanEmitterComponent::Auth_SetTriggered(bool value)
{
	if (!Owner->HasAuthority()) return;

	bTriggered = value;
	UE_LOG(LogTemp, Warning, TEXT("bTriggered set to %s by SetTriggered"), bTriggered ? TEXT("true") : TEXT("false"));
}

void UHitscanEmitterComponent::Auth_StartReloading()
{
	if (!Owner->HasAuthority()) return;

	bReloading = true;

	// 임시로 3초후에 리로딩 완료해주는것
	FTimerHandle TempReloadHandle;
	GetWorld()->GetTimerManager().SetTimer(TempReloadHandle, this, &UHitscanEmitterComponent::Auth_EndReloading, 3.0f, false);
}

void UHitscanEmitterComponent::Auth_EndReloading()
{
	bReloading = false;

	// 총알 채우기
	SetCurrentBullet(Owner->GetMaxBullet());

	Owner->ClientRPC_ReloadEnd(CurrentBullet);
}

void UHitscanEmitterComponent::Fire_Requester(int bulletCount)
{
	// 총알 개수 적용
	SetCurrentBullet(bulletCount);

	UE_LOG(LogTemp, Warning, TEXT("Fire_Requester called - CurrentBullet %d"), CurrentBullet);

	// 카메라 쉐이크
	if (CameraShakeSourceComp && FireCameraShake)
	{
		// auto pc = GetWorld()->GetFirstPlayerController();
		// pc->ClientStartCameraShake(FireCameraShake, 1.0f);
		CameraShakeSourceComp->StartCameraShake(FireCameraShake, 1.0f);
	}
}

void UHitscanEmitterComponent::Fire_Everyone(FVector hitLocation)
{
	// 이펙트
	if (FireParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireParticle, hitLocation);
	}
}

void UHitscanEmitterComponent::ReloadEnd_Requester(int bulletCount)
{
	// 총알 개수 적용
	SetCurrentBullet(bulletCount);

	bReloading = false;
}

void UHitscanEmitterComponent::DebugInfo()
{
	const FString bEnabledString = bEnabled ? TEXT("true") : TEXT("false");
	const FString bTriggeredString = bTriggered ? TEXT("true") : TEXT("false");
	const FString bReloadingString = bReloading ? TEXT("true") : TEXT("false");

	const FString logStr = FString::Printf(
		TEXT("bEnabeld: %s\nbTriggered: %s\nCurrentBullet: %d\nbReloading: %s\nCurrent Health: %f"),
		*bEnabledString,
		*bTriggeredString,
		CurrentBullet,
		*bReloadingString,
		Owner->GetHealth()
	);

	DrawDebugString(GetWorld(), Owner->GetActorLocation(), logStr, nullptr, FColor::Red, 0, true, 1);
}