// Fill out your copyright notice in the Description page of Project Settings.


#include "DuoShooting/Public/Player/TracerHero.h"
#include "Skill/TracerSkillSystemComponent.h"
#include "EnhancedInputComponent.h"
#include "Animations/TracerAnimInstance.h"
#include "UI/ShootingMainWidget.h"

// Sets default values
ATracerHero::ATracerHero()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetSkillSystemComponent(CreateDefaultSubobject<UTracerSkillSystemComponent>("SkillSystemComp"));
}

// Called when the game starts or when spawned
void ATracerHero::BeginPlay()
{
	// 기본값 설정 // 생성자에서 하면 네트워크 동기화가 안돼서 BeginPlay로 이전
	MaxBullet = 40;
	MaxHealth = 175;
	CurrentHealth = 175;

	UE_LOG(LogTemp, Warning, TEXT("ATracerHero BeginPlay with MaxBullet %d"), MaxBullet);

	Super::BeginPlay();

	TracerSkillSystemComp = Cast<UTracerSkillSystemComponent>(GetSkillSystemComponent());
	TracerAnimInstance = Cast<UTracerAnimInstance>(GetMesh()->GetAnimInstance());
}

void ATracerHero::InputMove(const struct FInputActionValue& value)
{
	if (TracerSkillSystemComp->GetCurrentSkillState() == ETracerSkillState::BLINK) return;
	if (TracerSkillSystemComp->GetCurrentSkillState() == ETracerSkillState::RECALL) return;

	Super::InputMove(value);
}

void ATracerHero::InputLook(const struct FInputActionValue& value)
{
	if (TracerSkillSystemComp->GetCurrentSkillState() == ETracerSkillState::BLINK) return;
	if (TracerSkillSystemComp->GetCurrentSkillState() == ETracerSkillState::RECALL) return;

	Super::InputLook(value);
}

void ATracerHero::InputJump(const struct FInputActionValue& value)
{
	if (TracerSkillSystemComp->GetCurrentSkillState() == ETracerSkillState::BLINK) return;
	if (TracerSkillSystemComp->GetCurrentSkillState() == ETracerSkillState::RECALL) return;

	Super::InputJump(value);
}

// Called every frame
void ATracerHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATracerHero::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATracerHero::DoAfterAction(EHeroActionType actionType)
{
	switch (actionType)
	{
	case EHeroActionType::NormalAttackStart:
		break;
	case EHeroActionType::NormalAttackEnd:
		break;
	case EHeroActionType::NormalAttackSuccess:
		if (TracerAnimInstance) TracerAnimInstance->PlayFireMontage();
		break;
	default:
		break;
	}
}