// Fill out your copyright notice in the Description page of Project Settings.

#include "DuoShooting/Public/Player/HeroBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Attack/HitscanEmitterComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "UI/ShootingMainWidget.h"
#include "DuoShooting/Public/Skill/SkillSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AHeroBase::AHeroBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 리소스 찾기
	// IMC 등록
	{
		ConstructorHelpers::FObjectFinder<UInputMappingContext> TempIMC(
			TEXT("'/Game/DuoShooting/Inputs/HeroDefaults/IMC_HeroDefault.IMC_HeroDefault'"));
		if (TempIMC.Succeeded()) { IMC_HeroDefault = TempIMC.Object; }
	}
	// IA 등록
	{
		ConstructorHelpers::FObjectFinder<UInputAction> TempIA(
			TEXT("'/Game/DuoShooting/Inputs/HeroDefaults/IA_HeroMove.IA_HeroMove'"));
		if (TempIA.Succeeded()) { IA_Move = TempIA.Object; }
	}
	{
		ConstructorHelpers::FObjectFinder<UInputAction> TempIA(
			TEXT("'/Game/DuoShooting/Inputs/HeroDefaults/IA_HeroLook.IA_HeroLook'"));
		if (TempIA.Succeeded()) { IA_Look = TempIA.Object; }
	}
	{
		ConstructorHelpers::FObjectFinder<UInputAction> TempIA(
			TEXT("'/Game/DuoShooting/Inputs/HeroDefaults/IA_HeroJump.IA_HeroJump'"));
		if (TempIA.Succeeded()) { IA_Jump = TempIA.Object; }
	}
	{
		ConstructorHelpers::FObjectFinder<UInputAction> TempIA(
			TEXT("'/Game/DuoShooting/Inputs/HeroDefaults/IA_HeroFire.IA_HeroFire'"));
		if (TempIA.Succeeded()) { IA_Fire = TempIA.Object; }
	}
	{
		ConstructorHelpers::FClassFinder<UShootingMainWidget> TempWidget(
			TEXT("'/Game/DuoShooting/UIs/WBP_ShootingMainWidget.WBP_ShootingMainWidget_C'"));
		if (TempWidget.Succeeded()) { ShootingMainWidgetFactory = TempWidget.Class; }
	}

	// 스피드 적용
	GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed;

	// 카메라 생성
	FirstPersonCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComp->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComp->SetRelativeLocation(FVector(0.0f, 0.0f, 80.f));
	FirstPersonCameraComp->bUsePawnControlRotation = true;

	// 히트스캔 발사기 컴포넌트 생성
	HitscanEmitterComp = CreateDefaultSubobject<UHitscanEmitterComponent>(TEXT("TestHitScanComponent"));
}

void AHeroBase::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// 공통 IMC 등록
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_HeroDefault, 0);
		}
	}
}

// Called when the game starts or when spawned
void AHeroBase::BeginPlay()
{
	Super::BeginPlay();

	// 슈팅 위젯 생성
	if (ShootingMainWidgetFactory)
	{
		ShootingMainWidget = CreateWidget<UShootingMainWidget>(GetWorld(), ShootingMainWidgetFactory);
		if (ShootingMainWidget != nullptr)
		{
			ShootingMainWidget->AddToViewport(); // ZOrder?
		}
	}
}

// Called every frame
void AHeroBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AHeroBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 공통 인풋 등록
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Started, this, &AHeroBase::InputJump);
		EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AHeroBase::InputLook);
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AHeroBase::InputMove);
		EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Started, this, &AHeroBase::InputFire_Enter);
		EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Completed, this, &AHeroBase::InputFire_Exit);
	}

	//스킬 시스템에 Input정보 넘기기
	InitSkillSystemInput(PlayerInputComponent);
}

void AHeroBase::InputMove(const FInputActionValue& value)
{
	FVector2D MovementVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AHeroBase::InputLook(const FInputActionValue& value)
{
	FVector2D LookAxisVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(-LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AHeroBase::InputJump(const FInputActionValue& value)
{
	Jump();
}

void AHeroBase::InputFire_Enter(const FInputActionValue& value)
{
	if (HitscanEmitterComp) HitscanEmitterComp->StartTrigger();
}

void AHeroBase::InputFire_Exit(const FInputActionValue& value)
{
	if (HitscanEmitterComp) HitscanEmitterComp->EndTrigger();
}

void AHeroBase::InitSkillSystemInput(class UInputComponent* playerInputComponent)
{
	if (auto* input = Cast<UEnhancedInputComponent>(playerInputComponent))
	{
		if (nullptr != SkillSystemComp)
			SkillSystemComp->SetupHeroInfo(this, input);
		else
			UE_LOG(LogTemp, Error, TEXT("SkillSystemComp is null!!!!! You must initialize SkillSystemComp!!!!!"));
	}
}

void AHeroBase::AddCurrentHeroState(EHeroState newState)
{
	int32 newStateBitmask = 1 << (int8)newState;
	CurrentHeroState |= newStateBitmask;
}

void AHeroBase::RemoveCurrentHeroState(EHeroState oldState)
{
	int32 oldStateBitmask = 1 << (int8)oldState;
	CurrentHeroState &= (~oldStateBitmask);
}

USkillSystemComponent* AHeroBase::GetSkillSystemComponent() const
{
	return SkillSystemComp;
}

TArray<EHeroState> AHeroBase::GetCurrentHeroState()
{
	TArray<EHeroState> result;
	for (int8 i = 0; i < (int8)EHeroState::LastIndex; i++)
	{
		if (CurrentHeroState & (1 << i))
			result.Add((EHeroState)i);
	}
	return result;
}

// 내가 아닌 다른 공격이 부를 함수
void AHeroBase::ApplyDamage(float damage, FDamageEvent const& damageEvent, AController* instigator,
                          AActor* damageCauser)
{
	// 피해를 준 컨트롤러와 액터 정보가 전송되므로 팀 관련 처리시 이부분이 쓰일지도?
	float actualDamage = TakeDamage(damage, damageEvent, instigator, damageCauser);
	UE_LOG(LogTemp, Warning, TEXT("%s가 %f만큼의 데미지를 입었습니다"), *GetName(), actualDamage);
	ReduceHealth(actualDamage);
}

void AHeroBase::SetHealth(float hp)
{
	CurrentHealth = FMath::Clamp(hp, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("%s의 체력이 %f가 되었습니다"), *GetName(), CurrentHealth);
}

void AHeroBase::ReduceHealth(float hp)
{
	SetHealth(CurrentHealth - hp);
}
