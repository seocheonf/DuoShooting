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
#include "Kismet/KismetMathLibrary.h"
#include "Management/TeamFightGameMode.h"
#include "Management/TeamFightGameState.h"
#include "Management/TeamFightPlayerState.h"
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
	FirstPersonCameraComp->PostProcessSettings.bOverride_ColorSaturation = true;

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
	HealthBarWidgetComp->SetRelativeLocation(FVector(0.0f, 0.0f, 114.0f));
	HealthBarWidgetComp->SetRelativeScale3D(FVector(1.0f, 0.2f, 0.2f));
	HealthBarWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ConstructorHelpers::FClassFinder<UUserWidget> TempHealthBar(
		TEXT("'/Game/DuoShooting/UIs/WBP_HealthBarWidget.WBP_HealthBarWidget_C'"));
	if (TempHealthBar.Succeeded()) { HealthBarWidgetComp->SetWidgetClass(TempHealthBar.Class); }
	HealthBarWidgetComp->SetDrawSize(FVector2D(1000.0f, 500.0f));
}

void AHeroBase::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// 공통 IMC 등록
	// 본인이 곧 새로운 플레이어 컨트롤러의 빙의 대상이 되었을 때
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// 이전의 모든 입력 맵핑 정보를 초기화 시키고, 새롭게 시작한다.
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(IMC_HeroDefault, 0);
		}
	}
}

void AHeroBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 배치파일로 실행시 서버쪽에는 고장난 메인위젯이 하나(?) 더 깔리는 오류가 생겨 추가해봄
	DestroyShootingMainWidget();
	
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
}

// Called when the game starts or when spawned
void AHeroBase::BeginPlay()
{
	Super::BeginPlay();

	// 스피드 적용
	GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed;
	GetCharacterMovement()->AirControl = 0.9f;

	UE_LOG(LogTemp, Warning, TEXT("[%s] [%d]Begin Play ---------------"), GetNetMode()==NM_Client?TEXT("Clienit"):TEXT("Server"),IsLocallyControlled());
	// 로컬이면 체력바를 끄고 메인위젯을 생성
	if (IsLocallyControlled())
	{
		if (HealthBarWidgetComp) HealthBarWidgetComp->SetVisibility(false);

		CreateShootingMainWidget();
	}
	// 로컬이 아니면 체력바를 초기화
	else
	{
		CreateHealthBarWidget();
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
	//PrintTeamLog();
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

#if WITH_EDITOR

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

void AHeroBase::PrintTeamLog()
{
	if (ATeamFightPlayerState* teamFightPlayerState = Cast<ATeamFightPlayerState>(GetPlayerState()))
	{
		const FString logStr = FString::Printf(
			TEXT("Team: %s"),
			*UEnum::GetValueAsString(teamFightPlayerState->GetPlayerTeam())
		);

		DrawDebugString(GetWorld(), GetActorLocation(), logStr, nullptr, FColor::Yellow, 0, true, 1);
	}
}

#endif


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

void AHeroBase::UpdateCurrentHealthUI() const
{
	if (ShootingMainWidget) // 주인공인 경우 메인 UI를 업데이트
		ShootingMainWidget->SetCurrentHealth(CurrentHealth);
	else if (HealthBarWidget) // 주인공이 아닌경우 머리위 체력바를 업데이트
		HealthBarWidget->SetCurrentHealth(CurrentHealth);
	else
		UE_LOG(LogTemp, Warning, TEXT("ShootingMainWidget Null"));
}

void AHeroBase::CreateHealthBarWidget()
{
	if (HealthBarWidgetComp)
	{
		HealthBarWidgetComp->InitWidget();
		HealthBarWidget = Cast<UHealthBarWidget>(HealthBarWidgetComp->GetWidget());
		if (HealthBarWidget)
		{
			HealthBarWidget->InitMaxHealth(MaxHealth);
			HealthBarWidget->SetCurrentHealth(CurrentHealth);
			UpdateUserNameUI();
		}
	}

	// 태어났을 때 나의 체력바의 팀을 설정 (로컬 플레이어보다 뒤늦게 들어온 경우)
	ApplyTeamVisuals();
}

void AHeroBase::CreateShootingMainWidget()
{
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
				
			// 메인위젯에 점수와 팀정보를 전달
			UpdateMyScoreUI();
			UpdateTeamScoreUI();

			// 월드에 이미 있는 모든 비로컬 플레이어들을 순회하며 팀 UI를 설정해주자
			TArray<AActor*> foundHeroes;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHeroBase::StaticClass(), foundHeroes);
			for (AActor* actor : foundHeroes)
			{
				AHeroBase* hero = Cast<AHeroBase>(actor);
				if (hero && !hero->IsLocallyControlled())
				{
					hero->ApplyTeamVisuals();
				}
			}
		}
	}
}

void AHeroBase::DestroyShootingMainWidget()
{
	if (ShootingMainWidget)
	{
		if (ShootingMainWidget->IsInViewport())
		{
			ShootingMainWidget->RemoveFromParent();
		}
		ShootingMainWidget = nullptr;
	}
}

void AHeroBase::UpdateMyScoreUI() const
{
	// 플레이어 스테이트의 점수를 가져와서 UI에 업데이트한다
	if (ShootingMainWidget)
	{
		if (auto* teamFightPlayerState = Cast<ATeamFightPlayerState>(GetPlayerState()))
		{
			ShootingMainWidget->SetMyScore(teamFightPlayerState->GetMyScore());
			UE_LOG(LogTemp, Warning, TEXT("ShootingMainWidget Score Set To %d"), teamFightPlayerState->GetMyScore());
		}
	}
}

void AHeroBase::UpdateTeamScoreUI() const
{
	if (ATeamFightPlayerState* teamFightPlayerState = Cast<ATeamFightPlayerState>(GetPlayerState()))
	{
		if (ShootingMainWidget)
		{
			ShootingMainWidget->SetMyTeamScore(teamFightPlayerState->GetMyTeamScore());
			ShootingMainWidget->SetEnemyTeamScore(teamFightPlayerState->GetEnemyTeamScore());

			UE_LOG(LogTemp, Warning, TEXT("AHeroBase::UpdateTeamScoreUI UI Set with %d, %d values"),
			       teamFightPlayerState->GetMyTeamScore(), teamFightPlayerState->GetEnemyTeamScore());
		}
	}
}

// 서버쪽에서 실행할 부활 함수
void AHeroBase::Server_ReSpawn()
{
	UE_LOG(LogTemp, Warning, TEXT("Server_ReSpawn Called"));

	if (ATeamFightGameMode* teamFightGameMode = Cast<ATeamFightGameMode>(
		GetWorld()->GetAuthGameMode()))
	{
		if (APlayerController* playerController = Cast<APlayerController>(
			GetController()))
		{
			// 임시로 트레이서로 스폰되게 하자
			//teamFightGameMode->SetPlayerHero(playerController, EHeroInfo::Tracer);

			teamFightGameMode->RespawnPlayer(playerController);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Teamfight GameMode Null"));
	}
}

// 죽으면 재생할 효과들
void AHeroBase::DieAfterAction()
{
	// 랙돌
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->bPauseAnims = false;
	GetMesh()->SetSimulatePhysics(true);

	// 콜리전 끄기
	SetCollisionEnable(false);

	// 체력바 UI 끄기
	if (ShootingMainWidget)
	{
		ShootingMainWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	// 인풋 막기
	bUseControllerRotationYaw = false;
	if (APlayerController* playerController = Cast<APlayerController>(GetController()))
	{
		DisableInput(playerController);
	}

	// 죽음용 카메라 처리
	FirstPersonCameraComp->bUsePawnControlRotation = false;
	FirstPersonCameraComp->PostProcessSettings.ColorSaturation = FVector4(0.0f, 0.0f, 0.0f, 1.0f); // 흑백
	FirstPersonCameraComp->SetRelativeLocation(FVector(-260.0f, 0.0f, 230.0f)); // 카메라 위로 올리기
	FirstPersonCameraComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); // 플레이어 콜라이더에서 떼기

	// 카메라가 위에서 플레이어 내려다보기
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(FirstPersonCameraComp->GetComponentLocation(),
	                                                                 GetActorLocation());
	FirstPersonCameraComp->SetWorldRotation(LookAtRotation);

	OnDieCompleteDelegate.Broadcast();
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

	// 같은 팀이면 리턴
	ATeamFightPlayerState* instigatorPlayerState = Cast<ATeamFightPlayerState>(EventInstigator->PlayerState);
	if (ATeamFightPlayerState* teamFightPlayerState = Cast<ATeamFightPlayerState>(GetPlayerState()))
	{
		if (instigatorPlayerState != nullptr)
		{
			if (instigatorPlayerState->GetPlayerTeam() == teamFightPlayerState->GetPlayerTeam())
			{
				UE_LOG(LogTemp, Warning, TEXT("같은 팀이라 공격이 무시되었습니다"));
				return 0.0f;
			}
		}
	}
	
	float actualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Warning, TEXT("%s가 %f만큼의 데미지를 입었습니다"), *GetName(), actualDamage);
	AddHealth(-actualDamage);

	// 체력이 닳으면 죽게 하기
	if (CurrentHealth < 0.001f)
	{
		// 모두에게 죽음을 알리기
		MultiRPC_Die();

		// 3초 후 리스폰 예약
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &AHeroBase::Server_ReSpawn,
		                                       3.0f, false);

		// 공격자의 킬 스코어 올리기
		if (instigatorPlayerState != nullptr) instigatorPlayerState->AddOneScore();
	}

	// OnRep은 서버쪽에서 안불리므로 서버쪽은 여기서 불러주자
	OnRep_CurrentHealth();

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

// 로컬 플레이어가 아닌 다른 애들의 이름을 머리 위에 보여주기
void AHeroBase::UpdateUserNameUI() const
{
	if (IsLocallyControlled()) return;

	if (HealthBarWidget)
	{
		if (auto* teamFightPlayerState = Cast<ATeamFightPlayerState>(GetPlayerState()))
		{
			HealthBarWidget->SetUserName(teamFightPlayerState->GetUserName());
		}
	}
}

//==김형모==

void AHeroBase::SetMeshVisibility(bool bVisible)
{
	GetMesh()->SetVisibility(bVisible);
}

void AHeroBase::SetCollisionEnable(bool bEnable)
{
	GetCapsuleComponent()->SetCollisionEnabled(bEnable
		                                           ? ECollisionEnabled::QueryAndPhysics
		                                           : ECollisionEnabled::NoCollision);
}

void AHeroBase::ServerRPC_DoAfterAction_Implementation(EHeroActionType actionType)
{
	DoAfterAction(actionType);
}

void AHeroBase::MultiRPC_Die_Implementation()
{
	AddCurrentHeroState(EHeroState::Die);

	DieAfterAction();
}

UHealthBarWidget* AHeroBase::GetHealthBarUI()
{
	return HealthBarWidget;
}

// 로컬 플레이어의 팀 상태를 가져와서 적 UI 외형을 설정
void AHeroBase::ApplyTeamVisuals()
{
	UE_LOG(LogTemp, Warning, TEXT("[%s]ApplyTeamVisuals called"), GetNetMode() == NM_Client?TEXT("Client"):TEXT("Server"));

	if (IsLocallyControlled()) return;

	// 로컬 플레이어의 게임 스테이트 가져오기
	APlayerController* localPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (localPlayerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s]ApplyTeamVisuals: localPlayerController Null"), GetNetMode() == NM_Client?TEXT("Client"):TEXT("Server"));
		return;
	}

	APlayerState* localPlayerState = localPlayerController->PlayerState;
	if (localPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s]ApplyTeamVisuals: localPlayerState Null"), GetNetMode() == NM_Client?TEXT("Client"):TEXT("Server"));
		return;
	}

	ATeamFightPlayerState* localTeamFightPlayerState = Cast<ATeamFightPlayerState>(localPlayerState);
	if (localTeamFightPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s]ApplyTeamVisuals: localPlayerState cannot be cast to ATeamFightPlayerState"), GetNetMode() == NM_Client?TEXT("Client"):TEXT("Server"));
		return;
	}

	// 이 캐릭터의 플레이어 스테이트가 비어 있으면 리턴
	ATeamFightPlayerState* teamFightPlayerState = Cast<ATeamFightPlayerState>(GetPlayerState());
	if (teamFightPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s]ApplyTeamVisuals: teamFightPlayerState Null"), GetNetMode() == NM_Client?TEXT("Client"):TEXT("Server"));
		return;
	}

	// UI가 존재하는지 체크
	if (HealthBarWidget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s]ApplyTeamVisuals: HealthBarWidget Null"), GetNetMode() == NM_Client?TEXT("Client"):TEXT("Server"));
		return;
	}

	// 로컬 플레이어와 같은 팀이면 같은편 비주얼로 설정
	if (localTeamFightPlayerState->GetPlayerTeam() == teamFightPlayerState->GetPlayerTeam())
	{
		HealthBarWidget->ApplyMyTeamMode();
		UE_LOG(LogTemp, Warning, TEXT("[%s]ApplyTeamVisuals: Successfully Executed (Local Player Team)"), GetNetMode() == NM_Client?TEXT("Client"):TEXT("Server"));
	}
	// 로컬 플레이어와 다른팀이면 다른편 비주얼로 설정
	else
	{
		HealthBarWidget->ApplyEnemyTeamMode();
		UE_LOG(LogTemp, Warning, TEXT("[%s]ApplyTeamVisuals: Successfully Executed (Enemy Team)"), GetNetMode() == NM_Client?TEXT("Client"):TEXT("Server"))
	}
}

void AHeroBase::DoAfterAction(EHeroActionType actionType)
{
}


//==김형모==
