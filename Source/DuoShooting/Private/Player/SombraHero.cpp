// Fill out your copyright notice in the Description page of Project Settings.

#include "DuoShooting/Public/Player/SombraHero.h"

#include "Skill/SombraSkillSystemComponent.h"

// Sets default values
ASombraHero::ASombraHero()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetSkillSystemComponent(CreateDefaultSubobject<USombraSkillSystemComponent>("SkillSystemComp"));

	ConstructorHelpers::FObjectFinder<UMaterial> mat(TEXT("/Script/Engine.Material'/Game/DuoShooting/Maps/KHM/M_MannequinTest.M_MannequinTest'"));
	if (mat.Succeeded())
	{
		OriginSombraMaterial = mat.Object;
	}
	
	SombraMaterialInstance = GetMesh()->CreateDynamicMaterialInstance(0, OriginSombraMaterial);
	SombraMaterialInstance2 = GetMesh()->CreateDynamicMaterialInstance(1, OriginSombraMaterial);
}

// Called when the game starts or when spawned
void ASombraHero::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASombraHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASombraHero::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
}

void ASombraHero::SetAppearance()
{
	SetCollisionEnable(true);
	SetMeshVisibility(true);
}

void ASombraHero::SetDisAppearance()
{
	SetCollisionEnable(false);
	SetMeshVisibility(false);
}

void ASombraHero::EnterStealth()
{
	
}

void ASombraHero::ExitStealth()
{
}

void ASombraHero::SetStealthState(EStealthState newState)
{
}

void ASombraHero::SetVisibilityAlpha(float alpha)
{
	if (VisibilityTimerHandle.IsValid())
		GetWorldTimerManager().ClearTimer(VisibilityTimerHandle);
	float captureAlpha = SombraMaterialAlpha;
	CurrentAlphaTime = 0.f;
	
	GetWorldTimerManager().SetTimer(VisibilityTimerHandle, [&, captureAlpha]()->void
	{
		CurrentAlphaTime += GetWorld()->GetDeltaSeconds();
		SombraMaterialAlpha = FMath::Lerp(captureAlpha, alpha, CurrentAlphaTime/MaxAlphaTime);
		SombraMaterialInstance->SetScalarParameterValue(TEXT("Alpha"), SombraMaterialAlpha);
		if (CurrentAlphaTime >= MaxAlphaTime)
		{
			SombraMaterialInstance->SetScalarParameterValue(TEXT("Alpha"), alpha);
			SombraMaterialAlpha = alpha;
			GetWorldTimerManager().ClearTimer(VisibilityTimerHandle);
		}
	}, 0.0003f, FTimerManagerTimerParameters(true, true));
}

