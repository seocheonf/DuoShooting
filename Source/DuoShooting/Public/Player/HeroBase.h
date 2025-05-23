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
	UnVisible,
	LastIndex
};

UCLASS(Abstract)
class DUOSHOOTING_API AHeroBase : public ACharacter
{
	GENERATED_BODY()
	
	//=====재 정의 함수=====
public:
	// Sets default values for this character's properties
	AHeroBase();
private:
	virtual void NotifyControllerChanged() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input / InitSkillSystemInput 기능을 함께 수행합니다.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	//=====변수=====
private:
	//히어로 공통 속성
	UPROPERTY(EditAnywhere)	// 기본 체력
	float MaxHealth = 100.0f;
	UPROPERTY(EditAnywhere)	// 현재 체력
	float CurrentHealth = 100.0f;
	UPROPERTY(EditAnywhere)	// 최대 총알 개수
	float MaxBullet;
	UPROPERTY(EditAnywhere)	// 현재 총알 개수
	float CurrentBullet;
	UPROPERTY(EditAnywhere)	// 기본 스피드
	float DefaultSpeed = 400.0f;
	//히어로 공통 인풋
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputMappingContext* IMC_HeroDefault;
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_Move;
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_Look;
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_Jump;
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_Fire;
	// 1인칭 카메라
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FirstPersonCameraComp;
	// 히트스캔 발사기 컴포넌트
	UPROPERTY(VisibleAnywhere, Blueprintable, Category = Shooting)
	class UHitscanEmitterComponent* HitscanEmitterComp;
	// 슈팅 기본 UI (조준선, 내 체력, 스킬 등?)
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<class UShootingMainWidget> ShootingMainWidgetFactory;
	UPROPERTY()
	class UShootingMainWidget* ShootingMainWidget;
	//스킬 시스템 (영웅은 스킬 시스템의 세부 내용을 직접 알 필요가 없다고 판단)
	//스킬 시스템을 각 캐릭터마다 설정해 주세요. 적용할 스킬 시스템을 반환시켜주면 됩니다.
	class USkillSystemComponent* SkillSystemComp;
protected:
	//영웅 상태 (bitmask)
	int32 CurrentHeroState;
public:
	//=====함수=====
private:
	// 기본 인풋
	void InputMove(const struct FInputActionValue& value);
	void InputLook(const struct FInputActionValue& value);
	void InputJump(const struct FInputActionValue& value);
	void InputFire_Enter(const struct FInputActionValue& value);
	void InputFire_Exit(const struct FInputActionValue& value);
	// 체력 변경
	void SetHealth(float hp);
	void ReduceHealth(float hp);
	//스킬 시스템에 본인과 본인의 입력을 등록하는 함수입니다.
	void InitSkillSystemInput(class UInputComponent* playerInputComponent);
	//영웅 상태 bitmask 계산 추가
	void AddCurrentHeroState(EHeroState newState);
	//영웅 상태 bitmask 계산 제거
	void RemoveCurrentHeroState(EHeroState oldState);
protected:
	//일단은 참고만 할 수 있게 놔뒀습니다. 필요하면 상의 후 const를 지워주세요.
	USkillSystemComponent* GetSkillSystemComponent() const;
public:
	//영웅 상태 bitmask 결과. 영웅의 상태에 대한 정보가 필요할 때 사용합니다.
	TArray<EHeroState> GetCurrentHeroState();
	//데미지 입기
	void ApplyDamage(float damage, FDamageEvent const& damageEvent, AController* instigator, AActor* damageCauser);
	UCameraComponent* GetCamera() const { return FirstPersonCameraComp; }
};
