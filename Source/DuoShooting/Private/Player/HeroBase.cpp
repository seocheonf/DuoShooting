// Fill out your copyright notice in the Description page of Project Settings.

#include "DuoShooting/Public/Player/HeroBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Attack/HitscanEmitterComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeSourceComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/ShootingMainWidget.h"
#include "DuoShooting/Public/Skill/SkillSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UI/HealthBarWidget.h"


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
		ConstructorHelpers::FClassFinder<UShootingMainWidget> TempWidget(
			TEXT("'/Game/DuoShooting/UIs/WBP_ShootingMainWidget.WBP_ShootingMainWidget_C'"));
		if (TempWidget.Succeeded()) { ShootingMainWidgetFactory = TempWidget.Class; }
	}

	// 리플리케이트
	bReplicates = true;

	// 카메라 생성
	FirstPersonCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComp->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComp->SetRelativeLocation(FVector(0.0f, 0.0f, 80.f));

	// 로테이션 컨트롤 설정
	FirstPersonCameraComp->bUsePawnControlRotation = true;
	bUseControllerRotationYaw = true;

	// 히트스캔 발사기 컴포넌트 생성
	HitscanEmitterComp = CreateDefaultSubobject<UHitscanEmitterComponent>(TEXT("HitScanEmitter"));
	//HitscanEmitterComp->SetIsReplicated(true);

	// 카메라 흔들림 컴포넌트 생성
	CameraShakeSourceComp = CreateDefaultSubobject<UCameraShakeSourceComponent>(TEXT("CameraShakeSource"));
	CameraShakeSourceComp->SetupAttachment(FirstPersonCameraComp);

	// 체력바
	HealthBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBarWidgetComp->SetupAttachment(RootComponent);
	HealthBarWidgetComp->SetRelativeLocation(FVector(0.0f, 0.0f, 104.0f));
	HealthBarWidgetComp->SetRelativeScale3D(FVector(1.0f, 0.13f, 0.02f));
	HealthBarWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ConstructorHelpers::FClassFinder<UUserWidget> TempHealthBar(
		TEXT("'/Game/DuoShooting/UIs/WBP_HealthBarWidget.WBP_HealthBarWidget_C'"));
	if (TempHealthBar.Succeeded()) { HealthBarWidgetComp->SetWidgetClass(TempHealthBar.Class); }
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
	
	// 스피드 적용
	GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed;
	GetCharacterMovement()->AirControl = 0.9f;

	// 로컬이면 체력바를 끄고 메인위젯을 생성
	if (IsLocallyControlled())
	{
		if (HealthBarWidgetComp)
		{
			HealthBarWidgetComp->SetVisibility(false);
		}
		
		// 슈팅 위젯 생성
		if (ShootingMainWidgetFactory)
		{
			ShootingMainWidget = CreateWidget<UShootingMainWidget>(GetWorld(), ShootingMainWidgetFactory);
			if (ShootingMainWidget != nullptr)
			{
				ShootingMainWidget->AddToViewport(); // ZOrder?

				// 메인위젯에 총탄, 체력 기본값 전달
				ShootingMainWidget->InitMaxBullet(MaxBullet);
				ShootingMainWidget->InitMaxHealth(MaxHealth);
				ShootingMainWidget->SetCurrentHealth(CurrentHealth);
			}
		}
	}
	// 로컬이 아니면 체력바를 초기화
	else
	{
		if (HealthBarWidgetComp)
		{
			HealthBarWidgetComp->InitWidget();
			HealthBarWidget = Cast<UHealthBarWidget>(HealthBarWidgetComp->GetWidget());
			if (HealthBarWidget)
			{
				HealthBarWidget->InitMaxHealth(MaxHealth);
				HealthBarWidget->SetCurrentHealth(CurrentHealth);
			}
		}
	}

	// 히트스캔 컴포넌트에 필수정보 전달
	if (HitscanEmitterComp) HitscanEmitterComp->Initialize(ShootingMainWidget, CameraShakeSourceComp);
}

// Called every frame
void AHeroBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 체력바 빌보딩
	if (HealthBarWidgetComp && HealthBarWidgetComp->GetVisibleFlag())
	{
		FVector camLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
		FVector direction = camLocation - HealthBarWidgetComp->GetComponentLocation();
		FRotator lookAtRotation = FRotationMatrix::MakeFromX(direction).Rotator();
		lookAtRotation.Pitch = 0.f;
		lookAtRotation.Roll = 0.f;
		HealthBarWidgetComp->SetWorldRotation(lookAtRotation);
	}
	
#if WITH_EDITOR
	//PrintNetLog();
#endif
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
		HitscanEmitterComp->SetupHitscanInputInfo(EnhancedInputComponent);
	}

	//스킬 시스템에 Input정보 넘기기
	InitSkillSystemInput(PlayerInputComponent);
}

void AHeroBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHeroBase, CurrentHealth);
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

void AHeroBase::PrintNetLog()
{
	const FString isConnectedStr = GetNetConnection() ? TEXT("연결됨") : TEXT("연결되지 않음");
	const FString ownerNameStr = GetOwner() ? GetOwner()->GetName() : TEXT("Owner 존재하지 않음");

	const FString logStr = FString::Printf(
		TEXT("NetConnection: %s\nOwnerName: %s\nLocalRole: %s\nRemoteRole: %s"),
		*isConnectedStr,
		*ownerNameStr,
		*UEnum::GetValueAsString<ENetRole>(GetLocalRole()),
		*UEnum::GetValueAsString<ENetRole>(GetRemoteRole())
	);

	DrawDebugString(GetWorld(), GetActorLocation(), logStr, nullptr, FColor::Yellow, 0, true, 1);
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

void AHeroBase::OnRep_CurrentHealth()
{
	// 체력이 바뀔 때 해야 할 것들

	// UI에 체력 업데이트
	UpdateCurrentHealthUI();
}

void AHeroBase::UpdateCurrentHealthUI()
{
	if (ShootingMainWidget) // 주인공인 경우 메인 UI를 업데이트
		ShootingMainWidget->SetCurrentHealth(CurrentHealth);
	else if (HealthBarWidget) // 주인공이 아닌경우 머리위 체력바를 업데이트
		HealthBarWidget->SetCurrentHealth(CurrentHealth);
	else
		UE_LOG(LogTemp, Warning, TEXT("ShootingMaingWidget Null"));
}

void AHeroBase::Die()
{
	AddCurrentHeroState(EHeroState::Die);

	// 랙돌
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	// 인풋 막기
	if (APlayerController* playerController = Cast<APlayerController>(GetController()))
	{
		DisableInput(playerController);
	}
}


void AHeroBase::SetSkillSystemComponent(USkillSystemComponent* targetSystem)
{
	SkillSystemComp = targetSystem;
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

// 데미지 입기: 언리얼 내장 함수를 씁니다
float AHeroBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                            class AController* EventInstigator, AActor* DamageCauser)
{
	// 서버에서만 데미지 적용
	if (!HasAuthority()) return 0.0f;
	
	float actualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Warning, TEXT("%s가 %f만큼의 데미지를 입었습니다"), *GetName(), actualDamage);
	AddHealth(-actualDamage);

	UpdateCurrentHealthUI();

	return actualDamage;
}

float AHeroBase::GetHealth()
{
	return CurrentHealth;
}

void AHeroBase::SetHealth(float hp)
{
	CurrentHealth = FMath::Clamp(hp, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("%s의 체력이 %f가 되었습니다"), *GetName(), CurrentHealth);
}

void AHeroBase::AddHealth(float hp)
{
	SetHealth(CurrentHealth + hp);
}

//==김형모==

void AHeroBase::SetMeshVisibility(bool bVisible)
{
	GetMesh()->SetVisibility(bVisible);
}

void AHeroBase::SetCollisionEnable(bool bEnable)
{
	GetCapsuleComponent()->SetCollisionEnabled(bEnable ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}

void AHeroBase::ServerRPC_DoAfterAction_Implementation(EHeroActionType actionType)
{
	DoAfterAction(actionType);
}

UHealthBarWidget* AHeroBase::GetHealthBarUI()
{
	return HealthBarWidget;
}

void AHeroBase::DoAfterAction(EHeroActionType actionType)
{
}


//==김형모==
