// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack/HitscanEmitterComponent.h"

#include "Camera/CameraComponent.h"
#include "Engine/DamageEvents.h"
#include "Player/HeroBase.h"


// Sets default values for this component's properties
UHitscanEmitterComponent::UHitscanEmitterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Owner = nullptr;
	OwnerCamera = nullptr;
}


// Called when the game starts
void UHitscanEmitterComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<AHeroBase>(GetOwner());
	if (Owner)
	{
		OwnerCamera = Owner->GetCamera();
	}
	else UE_LOG(LogTemp, Error, TEXT("UHitscanComponent이 AHeroBase 주인을 찾지 못하고 카메라도 가져오지 못함"));
}


// Called every frame
void UHitscanEmitterComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bTriggered)
	{
		FireTimer += DeltaTime;
		if (FireTimer >= HitScanInterval)
		{
			SingleLineTrace();
			FireTimer = 0.0f;
		}
	}
}

void UHitscanEmitterComponent::SingleLineTrace()
{
	FHitResult Result;
	
	// 카메라에서 시작
	FVector Start = OwnerCamera->GetComponentLocation();

	// 카메라 앞방향에서 설정한만큼 흐트러지게 설정
	FVector ForwardVector = OwnerCamera->GetForwardVector();
	if (Spread > 0.0f)
	{
		float MaxAngleInDegrees = Spread;

		FVector RandomSpread = FMath::VRandCone(ForwardVector, FMath::DegreesToRadians(MaxAngleInDegrees));
		ForwardVector = RandomSpread;
	}
	
	FVector End = Start + ForwardVector * HitScanDistance;
	
	// 스스로는 무시
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	bool bHit = GetWorld()->LineTraceSingleByChannel(Result, Start, End, ECC_Visibility, Params);
	
	if (bHit)
	{
		// 테스트용 시각화
		DrawDebugLine(GetWorld(), Start, Result.Location, FColor::Green, false, 5.0f);

		if (auto* hero = Cast<AHeroBase>(Result.GetActor()))
		{
			// 전달할 FPointDamageEvent 구조체 구성
			FPointDamageEvent damageEvent;
			damageEvent.Damage = DamageAmount; // 얘는 빌트인 TakeDamage 내부에서 안쓰이긴 하는데 일단은 넣어두자
			damageEvent.HitInfo = Result;

			// 피해 주기
			hero->ApplyDamage(DamageAmount, damageEvent, Owner->GetController(), Owner);
		}
	}
}

void UHitscanEmitterComponent::StartTrigger()
{
	// 이미 트리거되어 있다면 리턴
	if (bTriggered) return;
	bTriggered = true;
}

void UHitscanEmitterComponent::EndTrigger()
{
	// 이미 트리거되어있지 않다면 리턴
	if (!bTriggered) return;
	bTriggered = false;
	FireTimer = 1000.0f; // 충분히 큰 수
}

