// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HeroBase.h"
#include "SombraHero.generated.h"

UENUM()
enum class EStealthState : uint8
{
	None,
	Detection,
	Hidden
};

UCLASS()
class DUOSHOOTING_API ASombraHero : public AHeroBase
{
	GENERATED_BODY()

	//=====재 정의 함수=====
public:
	// Sets default values for this character's properties
	ASombraHero();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	//==오버라이드 함수==
	
	//HeroBase의 기본 기능을 사용한다.
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	//그 타이밍에 해야할 일.
	virtual void DoAfterAction(EHeroActionType actionType) override;
	
	//=====변수=====
private:
	// 은신 여부
	bool bStealth;
	// 은신 세부 상태
	EStealthState StealthState;
	// 보여짐 전환 보간용 타이머
	FTimerHandle VisibilityTimerHandle;
	
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* FirstViewSkeletalMeshComp;
	// base material
	//UMaterial* OriginSombraMaterial;
	// 보여짐 전환 시 material 값 변경용
	TArray<class UMaterialInstanceDynamic*> SombraMaterialInstances;
	//UMaterialInstanceDynamic* SombraMaterialInstance;
	//UMaterialInstanceDynamic* SombraMaterialInstance2;

	// mesh별 애니메이션 instance
	class USombraAnimInstance* FirstViewSkeletalAnimInstance;
	class USombraAnimInstance* ThirdViewSkeletalAnimInstance;
	
	// 현재 material alpha 값
	float SombraMaterialAlpha = 1.0f;
	// 현재 alpha 변화 진행 정도
	float CurrentAlphaTime = 0.f;
	// alpha 변화 진행 정도 마무리
	float MaxAlphaTime = 0.25f;

	//은신 화면 효과 색상 (ColorGain (Camera->PostProcess->Color Grading->Global->Gain))
	//purple color
	const FVector4 DefaultStealthStateCameraColorGain = FVector4(1.0f, 0.5f, 1.0f, 1.0f);

	//일반 공격 중 여부 확인
	bool bNormalAttacking = false;
	
protected:
public:
	//=====함수=====
private:
	//은신에 의한 material alpha값 변환 명령
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_SetStealthStateVisibility(EStealthState newState);
	UFUNCTION(Client, Reliable)
	void ClientRPC_SetStealthCamera(bool bStealthCamera);

	//공격 성공 시점에 할 일
	void DoAttackSuccess();
	//재장전 시점에 할 일
	void DoReloadStart();

	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_PlayAttackMontage();
	UFUNCTION(Client, Reliable)
	void ClientRPC_PlayReloadMontage();
	
protected:
	//material alpha값 변환 (타이머 보간, 타이머 단일 실행)
	void SetVisibilityAlpha(float alpha);
	//모습과 충돌 드러냄 (모습은 현재 은신 여부에 영향을 받음)
	void SetAppearance();
	//모습과 충돌 숨김
	void SetDisAppearance();
	//죽으면 실행할 효과들
	void DieAfterAction() override;
public:
	//위치 변환기에 의한 모습과 충돌을 드러내고 숨기는 함수
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_SetAppearanceTP(bool bAppearance);
	//은신 진입 (은신 세부 상태 Hidden)
	void EnterStealth();
	//은신 탈출 (은신 세부 상태 None)
	void ExitStealth();
	//은신 세부 상태 반환
	EStealthState GetStealthState();
	//은신 세부 상태 변환
	void SetStealthState(EStealthState newState);
	
	//==고유 함수 영역==
private:
protected:
public:

	//==솜브라 순간이동 이펙트
private:
	//솜브라가 순간이동 전후로 사라지고 나타날 때 내뿜는 파티클
	TSubclassOf<class AActor> OriginTeleportAppearanceParticle;

	//==사운드
private:
	//은신 사운드
	class USoundBase* OriginSoundStealth;
	//은신 사운드 옵션
	class USoundAttenuation* OriginSoundAttenuationStealth;

	//순간이동 시작 종료 사운드 (타인에게만)
	class USoundBase* OriginSoundTPBeforeAfter;
	
	//은신 사운드는 모두에게!
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_PlaySoundStealth();
	
};
