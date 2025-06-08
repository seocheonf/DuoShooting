// Fill out your copyright notice in the Description page of Project Settings.


#include "DuoShooting/Public/Player/TracerHero.h"
#include "Skill/TracerSkillSystemComponent.h"
#include "EnhancedInputComponent.h"
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

	// 로컬이면 체력바를 끄고 메인위젯을 생성
	if (IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("ATracerHero BeginPlay LOCALLYLCONTROLLED"));
	}
	// 로컬이 아니면 체력바를 초기화
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ATracerHero BeginPlay NOT LOCALLYLCONTROLLED"));
	}

	TracerSkillSystemComp = Cast<UTracerSkillSystemComponent>(GetSkillSystemComponent());
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
