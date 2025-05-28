// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/TracerSkill/PulseBomb.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
APulseBomb::APulseBomb()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(BoxComp);
	
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->bShouldBounce = false;
	// ProjectileMovement->bRotationFollowsVelocity = true;
	// ProjectileMovement->ProjectileGravityScale = 1.0f; // Bomb drops over time
	// ProjectileMovement->InitialSpeed = 1000.f;
	// ProjectileMovement->MaxSpeed = 1000.f;
}

// Called when the game starts or when spawned
void APulseBomb::BeginPlay()
{
	Super::BeginPlay();

	BoxComp->OnComponentHit.AddDynamic(this, &APulseBomb::OnHit);
}

// Called every frame
void APulseBomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APulseBomb::OnHit(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComp,
	FVector normalImpulse, const FHitResult& hit)
{
	if (ProjectileMovementComp)
	{
		ProjectileMovementComp->StopMovementImmediately();
		ProjectileMovementComp->StopSimulating(hit);
	}
	
	CurrentState = EPulseBombState::ATTACHED;
}

void APulseBomb::Launch(FVector direction, float speed)
{
	// FActorSpawnParameters SpawnParams;
	// SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//
	// FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.f;
	// FRotator SpawnRotation = GetControlRotation(); // Or camera forward
	//
	// APulseBomb* Bomb = GetWorld()->SpawnActor<APulseBomb>(BombClass, SpawnLocation, SpawnRotation, SpawnParams);
	//
	// if (Bomb)
	// {
	// 	FVector LaunchDirection = SpawnRotation.Vector();
	// 	Bomb->LaunchBomb(LaunchDirection, 1500.f);
	// }
	direction.Normalize();
	ProjectileMovementComp->Velocity = direction * speed;

	CurrentState = EPulseBombState::FLYING;
}
