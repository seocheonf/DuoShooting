// Fill out your copyright notice in the Description page of Project Settings.

#include "DuoShooting/Public/Player/SombraHero.h"

#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Player/SombraAnimInstance.h"
#include "Skill/SombraSkillSystemComponent.h"
#include "UI/HealthBarWidget.h"

// Sets default values
ASombraHero::ASombraHero()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetSkillSystemComponent(CreateDefaultSubobject<USombraSkillSystemComponent>("SkillSystemComp"));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> sm(TEXT("'/Game/LargeFile/ParagonRevenant/Characters/Heroes/Revenant/Meshes/Revenant.Revenant'"));
	if (sm.Succeeded())
	{
		USkeletalMeshComponent* smc = GetMesh();
		smc->SetSkeletalMeshAsset(sm.Object);
	}

	/* Legacy Dynamic Mat Preparing
	// ConstructorHelpers::FObjectFinder<UMaterial> mat(TEXT("'/Game/LargeFile/ParagonRevenant/Characters/Heroes/Revenant/Materials/M_Revenant_Body.M_Revenant_Body'"));
	// if (mat.Succeeded())
	// {
	// 	OriginSombraMaterial = mat.Object;
	// }
	*/

	ConstructorHelpers::FClassFinder<UAnimInstance> animInstance(TEXT("'/Game/DuoShooting/Blueprints/Characters/Sombra/ABP_Revenant.ABP_Revenant_C'"));
	if (animInstance.Succeeded())
	{
		USkeletalMeshComponent* smc = GetMesh();
		smc->SetAnimInstanceClass(animInstance.Class);
	}

	FirstViewSkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstViewSkeletalMeshComp"));
	FirstViewSkeletalMeshComp->SetupAttachment(GetCapsuleComponent());
	FirstViewSkeletalMeshComp->SetRelativeLocation(FVector(-65, 0, -90));
	FirstViewSkeletalMeshComp->SetRelativeRotation(FRotator(0, -90, 0));
	FirstViewSkeletalMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (sm.Succeeded())
	{
		FirstViewSkeletalMeshComp->SetSkeletalMeshAsset(sm.Object);
	}
	if (animInstance.Succeeded())
	{
		FirstViewSkeletalMeshComp->SetAnimInstanceClass(animInstance.Class);
	}
	
	
	GetMesh()->bOwnerNoSee = true;
	FirstViewSkeletalMeshComp->bOnlyOwnerSee = true;
}

// Called when the game starts or when spawned
void ASombraHero::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i<GetMesh()->GetMaterials().Num(); i++)
	{
		UMaterialInstanceDynamic* materialInstanceDynmaic = GetMesh()->CreateDynamicMaterialInstance(i);
		SombraMaterialInstances.Add(materialInstanceDynmaic);
		UE_LOG(LogTemp, Warning, TEXT("ASombraHero::BeginPlay"));
	}
	// SombraMaterialInstance = GetMesh()->CreateDynamicMaterialInstance(0, OriginSombraMaterial);
	// SombraMaterialInstance2 = GetMesh()->CreateDynamicMaterialInstance(1, OriginSombraMaterial)

	FirstViewSkeletalAnimInstance = Cast<USombraAnimInstance>(FirstViewSkeletalMeshComp->GetAnimInstance());
	ThirdViewSkeletalAnimInstance = Cast<USombraAnimInstance>(GetMesh()->GetAnimInstance());
}

// Called every frame
void ASombraHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//커스텀 틱. 스텔스 여부와 공격 여부 확인
	if (bStealth && bNormalAttacking)
	{
		ExitStealth();
	}
	
}

// Called to bind functionality to input
void ASombraHero::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ASombraHero::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float superReturnValue = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	GetSkillSystemComponent()->TakeDamage();
	return superReturnValue;
}

void ASombraHero::DoAfterAction(EHeroActionType actionType)
{
	Super::DoAfterAction(actionType);

	switch (actionType)
	{
	case EHeroActionType::NormalAttackStart:
		bNormalAttacking = true;
		break;
		
	case EHeroActionType::NormalAttackEnd:
		bNormalAttacking = false;
		break;
		
	case EHeroActionType::NormalAttackSuccess:
		DoAttackSuccess();
		break;
	
	default:
		break;
	}
}


void ASombraHero::MultiRPC_SetStealthStateVisibility_Implementation(EStealthState newState)
{
	/* 나라면 무시
	if (nullptr != Cast<APlayerController>(Controller))
		 return;
	*/
	if (IsLocallyControlled())
		return;
	
	//은신으로 인한 머티리얼 변경
	switch (newState)
	{
	case EStealthState::None:
		SetVisibilityAlpha(1.f);
		break;
	case EStealthState::Detection:
		SetVisibilityAlpha(0.5f);
		break;
	case EStealthState::Hidden:
		SetVisibilityAlpha(0.f);
		break;
	}
}

void ASombraHero::ClientRPC_SetStealthCamera_Implementation(bool bStealthCamera)
{
	if (bStealthCamera)
	{
		GetCamera()->PostProcessSettings.bOverride_ColorGain = true;
		GetCamera()->PostProcessSettings.ColorGain = DefaultStealthStateCameraColorGain;
	}
	else
	{
		GetCamera()->PostProcessSettings.ColorGain = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		GetCamera()->PostProcessSettings.bOverride_ColorGain = false;
	}
}

//서버측에서 실행될 함수임이 다른 사용처에서 보장됨.
void ASombraHero::DoAttackSuccess()
{
	UE_LOG(LogTemp, Warning, TEXT("%d"), GetLocalRole());

	//애니메이션을 리슨서버 호스트와 플레이어에 뿌리기
	MultiRPC_PlayAttackMontage();
}

void ASombraHero::MultiRPC_PlayAttackMontage_Implementation()
{
	FirstViewSkeletalAnimInstance->PlayAttackMontage();
	ThirdViewSkeletalAnimInstance->PlayAttackMontage();
}

void ASombraHero::SetVisibilityAlpha(float alpha)
{
	//기존 변화 종료 및 가로채기. 한번에 하나의 alpha 변화만 있어야 자연스러울 것 같았음. 
	if (VisibilityTimerHandle.IsValid())
		GetWorldTimerManager().ClearTimer(VisibilityTimerHandle);
	//실행 시점에 시작 alpha와 목표 alpha를 잡아둠. 현재시간-목표시간 대비 비율로 보간할거라 그럼.
	float captureStartAlpha = SombraMaterialAlpha;
	float captrueGoalAlpha = alpha;
	//새롭게 타이머를 시작하니 0으로 초기화
	CurrentAlphaTime = 0.f;

	//필요한 값들을 캡쳐
	GetWorldTimerManager().SetTimer(VisibilityTimerHandle, [&, captureStartAlpha, captrueGoalAlpha]()->void
	{
		//시간 갱신
		CurrentAlphaTime += GetWorld()->GetDeltaSeconds();
		//시간 경과 비율로 시작-목표 alpha 값 보간
		SombraMaterialAlpha = FMath::Lerp(captureStartAlpha, captrueGoalAlpha, CurrentAlphaTime/MaxAlphaTime);
		//alpha 값 적용
		for (auto each : SombraMaterialInstances)
		{
			each->SetScalarParameterValue(TEXT("Alpha"), SombraMaterialAlpha);
		}
		//SombraMaterialInstance->SetScalarParameterValue(TEXT("Alpha"), SombraMaterialAlpha);
		//SombraMaterialInstance2->SetScalarParameterValue(TEXT("Alpha"), SombraMaterialAlpha);

		//시간 완료시
		if (CurrentAlphaTime >= MaxAlphaTime)
		{
			//목표 값을 정확히 적용
			for (auto each : SombraMaterialInstances)
			{
				each->SetScalarParameterValue(TEXT("Alpha"), captrueGoalAlpha);
			}
			//SombraMaterialInstance->SetScalarParameterValue(TEXT("Alpha"), captrueGoalAlpha);
			//SombraMaterialInstance2->SetScalarParameterValue(TEXT("Alpha"), captrueGoalAlpha);
			
			UHealthBarWidget* healthBarWidget = GetHealthBarUI();
			FLinearColor targetColor = healthBarWidget->GetColorAndOpacity();
			targetColor.A = captrueGoalAlpha;
			healthBarWidget->SetColorAndOpacity(targetColor);
			
			SombraMaterialAlpha = captrueGoalAlpha;
			//타이머 종료
			GetWorldTimerManager().ClearTimer(VisibilityTimerHandle);
		}
	}, 0.0003f, FTimerManagerTimerParameters(true, true));
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

void ASombraHero::DieAfterAction()
{
	Super::DieAfterAction();

	// 1인칭 메쉬 끄기
	FirstViewSkeletalMeshComp->SetVisibility(false, true);
	FirstViewSkeletalMeshComp->SetHiddenInGame(true);
	//FirstViewSkeletalMeshComp->SetComponentTickEnabled(false);

	// 3인칭 메쉬가 나에게도 보이게 하기
	GetMesh()->SetOwnerNoSee(false);
}

void ASombraHero::MultiRPC_SetAppearanceTP_Implementation(bool bAppearance)
{
	/*나라면 무시
	if (nullptr == Cast<APlayerController>(SombraPlayer->Controller))
		return;
	 */
	if (IsLocallyControlled())
		return;
	
	if (bAppearance)
	{
		SetAppearance();
	}
	else
	{
		SetDisAppearance();
	}
}

void ASombraHero::EnterStealth()
{
	bStealth = true;
	ClientRPC_SetStealthCamera(true);
	SetStealthState(EStealthState::Hidden);
}

void ASombraHero::ExitStealth()
{
	SetStealthState(EStealthState::None);
	ClientRPC_SetStealthCamera(false);
	bStealth = false;
}

EStealthState ASombraHero::GetStealthState()
{
	return StealthState;
}

void ASombraHero::SetStealthState(EStealthState newState)
{
	if (!bStealth)
		return;	
	
	StealthState = newState;

	MultiRPC_SetStealthStateVisibility(StealthState);
}

