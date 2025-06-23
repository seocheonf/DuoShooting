// Fill out your copyright notice in the Description page of Project Settings.


#include "DuoShooting/Public/Skill/SkillSystemComponent.h"

#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Player/HeroBase.h"
#include "Tool/CoolTimerManagerComponent.h"
#include "UI/SkillSystemBaseUI.h"

// Sets default values for this component's properties
USkillSystemComponent::USkillSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	ConstructorHelpers::FClassFinder<USkillSystemBaseUI> ui(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/DuoShooting/UIs/WBP_SkillSystemBaseUI.WBP_SkillSystemBaseUI_C'"));
	if (ui.Succeeded())
	{
		OriginSkillUI = ui.Class;
	}

	//스킬 사운드
	ConstructorHelpers::FObjectFinder<USoundBase> soundSkillOn(TEXT("/Script/Engine.SoundWave'/Game/DuoShooting/Sounds/Sombra/SkillOn.SkillOn'"));
	if (soundSkillOn.Succeeded())
	{
		OriginSoundSkillOn = soundSkillOn.Object;
	}
}


// Called when the game starts
void USkillSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	
	if (nullptr == TargetPlayer)
	{
		TargetPlayer = Cast<AHeroBase>(GetOwner());
	}

	if (TargetPlayer->IsLocallyControlled())
	{
		SkillUI = CreateWidget<USkillSystemBaseUI>(GetWorld(), OriginSkillUI);
		SkillUI->AddToViewport();

		TargetPlayer->OnDieCompleteDelegate.AddUObject(this, &USkillSystemComponent::DoAfterTargetPlayerDie);
	}
}

void USkillSystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (SkillUI != nullptr)
	{
		SkillUI->RemoveFromParent();
	}
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

void USkillSystemComponent::ReSetupHeroInputInfo()
{
	SetupHeroInfo(TargetPlayer, TargetPlayerEnhancedInputComponent);
}

void USkillSystemComponent::RemoveHeroInputInfo()
{
	// 현재 컨트롤러가 플레이컨트롤러가 맞다면
	if (auto* pc = Cast<APlayerController>(TargetPlayer->Controller))
	{
		// UEnhancedInputLocalPlayerSubsystem를 가져와서
		auto* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		// AddMappingContext를 하고싶다.
		subsystem->RemoveMappingContext(IMC_SkillSystem);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("not player remove input"));
	}
	UE_LOG(LogTemp, Error, TEXT("removeinput"));
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

AActor* USkillSystemComponent::GetTargetActor()
{
	return Cast<AActor>(TargetPlayer);
}

void USkillSystemComponent::TakeDamage()
{
	
}

int32 USkillSystemComponent::AddSkillUI(UTexture2D* skillIcon, FText skillKeyName)
{
	if (SkillUI == nullptr || skillIcon == nullptr)
		return -1;

	int32 currentSkillIndex = NextSkillIndex;
	NextSkillIndex++;

	SkillUI->AddSkill(skillIcon, skillKeyName);

	return currentSkillIndex;
}

void USkillSystemComponent::ClientRPC_SetSkillCoolTimeUI_Implementation(int32 index, float upper, float lower)
{
	if (SkillUI == nullptr)
		return;
	SkillUI->SetSkillCoolTimeUI(index, upper, lower);
}

void USkillSystemComponent::ClientRPC_SetSkillIconActivation_Implementation(int index, bool bActive, bool bForbidden)
{
	if (SkillUI == nullptr)
		return;
	SkillUI->SetActiveSkillIcon(index, bActive, bForbidden);
}

void USkillSystemComponent::ClientRPC_SetSkillRemainTimeUI_Implementation(int32 index, int32 remainTime, bool bEmpty)
{
	if (SkillUI != nullptr)
	{
		SkillUI->SetSkillRemainCoolTimeUI(index, remainTime, bEmpty);
	}
}

void USkillSystemComponent::DoAfterTargetPlayerDie()
{
	if (TargetPlayer != nullptr && SkillUI != nullptr)
	{
		SkillUI->SetVisibility(ESlateVisibility::Hidden);// ->RemoveFromParent();
	}
}

void USkillSystemComponent::RemoveSkillUI()
{
	if (SkillUI != nullptr)
	{
		SkillUI->RemoveFromParent();
	}
}

void USkillSystemComponent::SetSkillUI(USkillSystemBaseUI* skillUI)
{
	SkillUI = skillUI;
}

void USkillSystemComponent::AddAdditionalSkillUI(int32 index, UUserWidget* widget)
{
	if (SkillUI != nullptr)
	{
		SkillUI->AddAdditionalIcon(index, widget);
	}
}

void USkillSystemComponent::ClientRPC_PlaySoundSkillOn_Implementation()
{
	UGameplayStatics::PlaySound2D(GetWorld(), OriginSoundSkillOn);
}
