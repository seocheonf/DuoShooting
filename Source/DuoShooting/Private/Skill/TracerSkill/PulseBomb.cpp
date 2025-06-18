// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/TracerSkill/PulseBomb.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"


// Sets default values
APulseBomb::APulseBomb()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->SetIsReplicated(true);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(SphereComp);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->bShouldBounce = false;
	ProjectileMovementComp->SetUpdatedComponent(SphereComp);
	ProjectileMovementComp->SetIsReplicated(true);
	// ProjectileMovement->bRotationFollowsVelocity = true;
	// ProjectileMovement->ProjectileGravityScale = 1.0f; // Bomb drops over time
	// ProjectileMovement->InitialSpeed = 1000.f;
	// ProjectileMovement->MaxSpeed = 1000.f;

	{
		ConstructorHelpers::FObjectFinder<USoundBase> TempSound(
			TEXT("'/Game/StarterContent/Audio/Explosion02.Explosion02'"));
		if (TempSound.Succeeded()) { ExplosionSound = TempSound.Object; }
	}

	bReplicates = true;
	SetReplicatingMovement(true);
	
	ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(
		TEXT("'/Game/LargeFile/ParagonDrongo/FX/Particles/Abilities/Grenade/FX/P_Drongo_Grenade_Explode.P_Drongo_Grenade_Explode'"));
	if (ParticleAsset.Succeeded()) { FireParticle = ParticleAsset.Object; }
}

// Called when the game starts or when spawned
void APulseBomb::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() != ROLE_Authority)
	{
		ProjectileMovementComp->Deactivate();
		return;
	}
	SphereComp->OnComponentHit.AddDynamic(this, &APulseBomb::OnHit); // how do you unsubsribe, or unbind
}

// Called every frame
void APulseBomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APulseBomb::PlayExplosionEffects()
{
	if (FireParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireParticle, StaticMeshComp->GetComponentLocation());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FireParticle is null"));
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ExplosionSound is null"));
	}
}

// 클라이언트 사이드에서 처리해줄 것
void APulseBomb::OnRep_CurrentState()
{
	switch (CurrentState)
	{
	case EPulseBombState::FLYING:
		break;
	case EPulseBombState::ATTACHING:
		break;
	case EPulseBombState::ATTACHED:
		break;
	case EPulseBombState::EXPLODING:
		PlayExplosionEffects();
		UE_LOG(LogTemp, Error, TEXT("CurrentState changed to explision"));
		break;
	}
}

// 핵심 로직은 전부 서버에서 처리
void APulseBomb::Launch(FVector direction, float speed, AController* instigator)
{
	if (GetLocalRole() != ROLE_Authority) return;
	
	direction.Normalize();
	ProjectileMovementComp->Velocity = direction * speed;

	// 부착을 위해 조금 위로 기울어진 채로 던진다
	FRotator rot = GetActorRotation();
	rot.Pitch += rot.Pitch - 45.0f;
	SetActorRotation(rot);

	// 던진이 저장
	InstigatorController = instigator;

	CurrentState = EPulseBombState::FLYING;
}

void APulseBomb::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APulseBomb, CurrentState);
}

void APulseBomb::OnHit(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComp,
                       FVector normalImpulse, const FHitResult& hit)
{
	if (GetLocalRole() != ROLE_Authority) return;
	
	SphereComp->OnComponentHit.RemoveDynamic(this, &APulseBomb::OnHit);

	// 부딪히면
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->StopMovementImmediately();
		ProjectileMovementComp->StopSimulating(hit);
	}
	
	CurrentState = EPulseBombState::ATTACHING;

	// 목표물에 붙이기
	AttachToComponent(otherComp, FAttachmentTransformRules::KeepWorldTransform);
	
	//StaticMeshComp->SetWorldLocation(hit.ImpactPoint);
	
	// FVector SurfaceNormal = hit.ImpactNormal;
	// FRotator AttachRot = SurfaceNormal.ToOrientationRotator();
	// StaticMeshComp->SetWorldRotation(AttachRot);

	// 1초 후 터진다
	GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &APulseBomb::Explode, 1.0f, false);
}

void APulseBomb::Explode()
{
	if (GetLocalRole() != ROLE_Authority) return;
	
	CurrentState = EPulseBombState::EXPLODING;
	UE_LOG(LogTemp, Warning, TEXT("펄스 폭탄 터짐!"));

	PlayExplosionEffects();
	
	// 터지기
	// 반경 내의 Authority를 가진 액터들에게만 범위형 공격을 가합니다
	UGameplayStatics::ApplyRadialDamageWithFalloff(
		GetWorld(),
		MaximumDamage,
		MinimumDamage,
		GetActorLocation(),
		Radius_FullDamage,
		Radius,
		1.0f,
		UDamageType::StaticClass(),
		TArray<AActor*>(),
		this,
		InstigatorController
	);


	//// 시각화
	//DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 30, FColor::Red, false, 1.0f);

	// 없어지기
	StaticMeshComp->SetVisibility(false);
	
	// 일정 시간이 지난 뒤 없애자
	GetWorldTimerManager().SetTimer(
		ExplosionTimerHandle,
		[this]() { if (IsValid(this)) Destroy(); },
		3.0f,
		false
	);
}


