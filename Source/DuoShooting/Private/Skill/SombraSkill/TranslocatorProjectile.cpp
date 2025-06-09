// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/SombraSkill/TranslocatorProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Skill/SombraSkillSystemComponent.h"


// Sets default values
ATranslocatorProjectile::ATranslocatorProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(SphereComp);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> mesh(TEXT("'/Engine/BasicShapes/Sphere.Sphere'"));
	if (mesh.Succeeded())
	{
		OriginStaticMesh = mesh.Object;
	}

	ConstructorHelpers::FObjectFinder<UMaterial> mat(TEXT("'/Game/DuoShooting/Materials/Characters/Skill/Sombra/M_TranslocatorProjectile.M_TranslocatorProjectile'"));
	if (mat.Succeeded())
	{
		OriginMaterial = mat.Object; 
	}
	
	ConstructorInit();
}

// Called when the game starts or when spawned
void ATranslocatorProjectile::BeginPlay()
{
	Super::BeginPlay();

	CustomBeginPlay();
}

// Called every frame
void ATranslocatorProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if WITH_EDITOR
	if (!bInitial)
	{
		UE_LOG(LogTemp, Error, TEXT("You must call Initializer function once, (ATranslocatorProjectile::Tick)"));
		Destroy();
		return;
	}
#endif

	CurrentFlyingTime += DeltaTime;
	if (CurrentFlyingTime >= MaxFlyingTime && !bStop)
	{
		bStop = true;
		OnOperate();
	}
	
}

void ATranslocatorProjectile::ConstructorInit()
{
	SphereComp->SetCollisionProfileName(TEXT("TranslocatorProjectile"));
	SphereComp->SetSphereRadius(50.f);
	StaticMeshComp->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshComp->SetStaticMesh(OriginStaticMesh);

	bReplicates = true;
}

void ATranslocatorProjectile::CustomBeginPlay()
{
	SphereComp->OnComponentHit.AddDynamic(this, &ATranslocatorProjectile::OnComponentHit);
	SphereComp->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
	StaticMeshComp->SetMaterial(0, OriginMaterial);

	SetReplicateMovement(true);
}

void ATranslocatorProjectile::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	OnOperate();
}

void ATranslocatorProjectile::OnOperate()
{
	if (nullptr == Launcher)
		return;
	
	if (bOperation)
		return;
	
	MovementComp->Velocity = FVector(0, 0, 0);
	MovementComp->ProjectileGravityScale = 0.f;
	MovementComp->bSimulationEnabled = false;

	Launcher->TriggerTranslocator(GetActorLocation());
	
	FTimerHandle timerHandle;
	GetWorldTimerManager().SetTimer(timerHandle, [&]()->void
	{
		Destroy();
	}, 0.25f, false);

	bOperation = true;
}

void ATranslocatorProjectile::Initializer(class USombraSkillSystemComponent* launcher, const FVector& launchPoint,
	const FVector& launchDirection, const float launchSpeed, const float flyingTime)
{
	
#if WITH_EDITOR
	if (bInitial)
	{
		UE_LOG(LogTemp, Error, TEXT("Don't call Initializer function more than 2 times. (ATranslocatorProjectile::Initializer)"));
		return;
	}
#endif
	
	Launcher = launcher;
	SetActorLocation(launchPoint);
	MovementComp->Velocity = launchDirection * launchSpeed;
	MaxFlyingTime = flyingTime;

#if WITH_EDITOR
	bInitial = true;
#endif
}

