// Fill out your copyright notice in the Description page of Project Settings.


#include "DuoShooting/Public/Skill/SkillSystemComponent.h"

#include "EnhancedInputSubsystems.h"
#include "Player/HeroBase.h"
#include "Tool/CoolTimerManagerComponent.h"


// Sets default values for this component's properties
USkillSystemComponent::USkillSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	
	CoolTimerManagerComp = CreateDefaultSubobject<UCoolTimerManagerComponent>(TEXT("CoolTimerManagerComp"));

}


// Called when the game starts
void USkillSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USkillSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USkillSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void USkillSystemComponent::SetupHeroInfo(class AHeroBase* targetPlayer,
                                          class UEnhancedInputComponent* enhancedInputComponent)
{
	TargetPlayer = targetPlayer;
	// 현재 컨트롤러가 플레이컨트롤러가 맞다면
	if (auto* pc = Cast<APlayerController>(TargetPlayer->Controller))
	{
		// UEnhancedInputLocalPlayerSubsystem를 가져와서
		auto* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		// AddMappingContext를 하고싶다.
		subsystem->RemoveMappingContext(IMC_SkillSystem);
		subsystem->AddMappingContext(IMC_SkillSystem, 0);
	}
	SetupHeroInputInfo(enhancedInputComponent);
}

void USkillSystemComponent::SetCurrentUltimateSkillGauge(int32 skillGauge)
{
	CurrentUltimateSkillGauge = skillGauge;
}

int32 USkillSystemComponent::GetCurrentUltimateSkillGauge()
{
	return CurrentUltimateSkillGauge;
}

int32 USkillSystemComponent::GetUltimateSkillGaugePercent()
{
	float ratio = CurrentUltimateSkillGauge / MaxUltimateSkillGauge;
	return ratio * 100;
}

void USkillSystemComponent::ChargeUltimateGauge(int32 amount)
{
	//궁극기 시전 중이거나 지속 중에는 궁극기 게이지가 차지 않음. 
	if (bUltimateSkillCasting || bUltimateSkillMaintaining)
		return;
	
	int32 cur = GetCurrentUltimateSkillGauge();
	cur += amount;
	cur = FMath::Min(cur, MaxUltimateSkillGauge);
	SetCurrentUltimateSkillGauge(cur);
}


