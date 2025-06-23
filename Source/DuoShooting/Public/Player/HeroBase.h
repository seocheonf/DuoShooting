// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HeroBase.generated.h"

UENUM()
enum class EHeroState : uint8
{
	Idle = 0,
	Attack,
	Die,
	NoSkill,
	InVisible,
	NoCollsision,
	LastIndex
};

UENUM()
enum class EHeroActionType : uint8
{
	NormalAttackStart,
	NormalAttackEnd,
	NormalAttackSuccess,
	Skill1,
	Skill2,
	ReloadStart
};

//죽은 후 할일 후 실행할 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnDieCompleteDelegate);

UCLASS(Abstract)
class DUOSHOOTING_API AHeroBase : public ACharacter
{
	GENERATED_BODY()
	
	//=====재 정의 함수=====
public:
	// Sets default values for this character's properties
	AHeroBase();
private:
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void NotifyControllerChanged() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input / InitSkillSystemInput 기능을 함께 수행합니다.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	//=====변수=====
private:
	//히어로 공통 인풋
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputMappingContext* IMC_HeroDefault;
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_Move;
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_Look;
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_Jump;
	// 1인칭 카메라
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FirstPersonCameraComp;
	// 히트스캔 발사기 컴포넌트
	UPROPERTY(VisibleAnywhere, Blueprintable, Category = Shooting)
	class UHitscanEmitterComponent* HitscanEmitterComp;
	// 카메라 흔들림 컴포넌트
	UPROPERTY()
	class UCameraShakeSourceComponent* CameraShakeSourceComp;
	// 슈팅 기본 UI (조준선, 내 체력, 스킬 등?)
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UShootingMainWidget> ShootingMainWidgetFactory;
	//스킬 시스템 (영웅은 스킬 시스템의 세부 내용을 직접 알 필요가 없다고 판단)
	//스킬 시스템을 각 캐릭터마다 설정해 주세요. 적용할 스킬 시스템을 반환시켜주면 됩니다.
	class USkillSystemComponent* SkillSystemComp;
	// 죽고나서 리스폰될때까지 타이머
	FTimerHandle RespawnTimerHandle;
#if WITH_EDITOR
	// 네트워크 상태 로그 출력
	void PrintNetLog();
	// 팀플 관련 로그 출력
	void PrintTeamLog();
#endif
protected:
	//히어로 공통 속성
	UPROPERTY(EditAnywhere)	// 기본 체력
	float MaxHealth = 100.0f;
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth)	// 현재 체력 - 꼭 SetHealth를 통해 바꿀것
	float CurrentHealth = 100.0f;
	UPROPERTY(EditAnywhere)	// 최대 총알 개수
	int32 MaxBullet = 100;
	UPROPERTY(EditAnywhere)	// 기본 스피드
	float DefaultSpeed = 400.0f;
	// 공통 UI (체력, 총탄, 조준선 등)
	UPROPERTY()
	class UShootingMainWidget* ShootingMainWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UWidgetComponent* HealthBarWidgetComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UHealthBarWidget* HealthBarWidget;
	//영웅 상태 (bitmask)
	int32 CurrentHeroState;
public:
	//=====함수=====
private:
	//스킬 시스템에 본인과 본인의 입력을 등록하는 함수입니다.
	void InitSkillSystemInput(class UInputComponent* playerInputComponent);
	//영웅 상태 bitmask 계산 추가
	void AddCurrentHeroState(EHeroState newState);
	//영웅 상태 bitmask 계산 제거
	void RemoveCurrentHeroState(EHeroState oldState);
	UFUNCTION()
	void OnRep_CurrentHealth();
	void UpdateCurrentHealthUI() const;
	void CreateHealthBarWidget();
	void CreateShootingMainWidget();
	void DestroyShootingMainWidget();
protected:
	// 서버쪽에서 실행할 부활 함수
	virtual void Server_ReSpawn();
	// 죽으면 재생할 효과들
	virtual void DieAfterAction();
	// 기본 인풋(이동, 회전, 점프)
	virtual void InputMove(const struct FInputActionValue& value);
	virtual void InputLook(const struct FInputActionValue& value);
	virtual void InputJump(const struct FInputActionValue& value);
	//스킬 컴포넌트를 상속받은 클래스에서 설정할 수 있도록 했습니다.
	void SetSkillSystemComponent(USkillSystemComponent* targetSystem);
public:
	//일단은 참고만 할 수 있게 놔뒀습니다. 필요하면 상의 후 const를 지워주세요.
	USkillSystemComponent* GetSkillSystemComponent() const;
public:
	//영웅 상태 bitmask 결과. 영웅의 상태에 대한 정보가 필요할 때 사용합니다.
	TArray<EHeroState> GetCurrentHeroState();
	UCameraComponent* GetCamera() const { return FirstPersonCameraComp; }
	UHitscanEmitterComponent* GetHitscanEmitter() const { return HitscanEmitterComp; }
	UShootingMainWidget* GetShootingMainWidget() const { return ShootingMainWidget; }
	float GetDefaultSpeed() const { return DefaultSpeed; }
	//체력 Get/Set
	float GetHealth();
	void SetHealth(float hp);
	void AddHealth(float hp);
	// PJW: 유저이름, 스코어 등의 UI 업데이트를 플레이어 스테이트나 게임 스테이트 내부에서 처리하지 않고 히어로의 함수로 처리한 이유는, 히어로가 태어날 때 UI 초기화를 위해 BeginPlay에서도 인위적으로 불러 주기 위함
	//유저이름 UI 업데이트하기
	void UpdateUserNameUI() const;
	//개인 스코어 UI 업데이트하기
	void UpdateMyScoreUI() const;
	//개인 스코어 UI 업데이트하기
	void UpdateTeamScoreUI() const;
	//데미지 입기
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	//최대 총알 Get
	int32 GetMaxBullet() const { return MaxBullet; }
	
	//==김형모==

public:
	//캐릭터 메시 시각처리 켜고 끄기
	void SetMeshVisibility(bool bVisible);
	//캐릭터 충돌체 켜고(hit, overlap 충돌 처리) 끄기
	void SetCollisionEnable(bool bEnable);

	//서버에서 DoAfterAction을 처리할 수 있도록 DoAfterAction을 감싼 함수
	UFUNCTION(Server, Reliable)
	void ServerRPC_DoAfterAction(EHeroActionType actionType);

	// 이 캐릭터가 죽으면 모든 클라이언트 사이드에게 알림
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_Die();

	// 체력바(타인용) UI 가져오기
	UHealthBarWidget* GetHealthBarUI();
	
	// 로컬 플레이어 기준에서 이 캐릭터의 적 여부를 UI에 적용하기 위해 부를 함수 
	void ApplyTeamVisuals();
	
protected:
	//캐릭터가 EHeroActionType에 따라 그 타이밍 이후에 곧장 할 일
	virtual void DoAfterAction(EHeroActionType actionType);

	//==김형모==
public:
	FOnDieCompleteDelegate OnDieCompleteDelegate;
};
