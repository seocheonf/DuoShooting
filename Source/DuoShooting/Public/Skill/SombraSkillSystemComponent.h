// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "SkillSystemComponent.h"
#include "SombraSkillSystemComponent.generated.h"

UENUM()
enum class EDetection
{
	PlayerDetection = 0,
	HitDetection = 1,
	HackDetection = 2
};

USTRUCT()
struct FHackTargetCalInfo
{
	GENERATED_BODY()
	//기본 정보
	float alpha = 50.f;
	float beta = 30.f;
	float da = 500.f;
	float db = 1250.f;
	//계산 정보
	float ra;
	float dbp;
	float rb;
	FVector SecondHackStartPoint;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUOSHOOTING_API USombraSkillSystemComponent : public USkillSystemComponent
{
	GENERATED_BODY()

	//=====재 정의 함수=====
public:
	// Sets default values for this component's properties
	USombraSkillSystemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	//=====변수=====
private:
	//솜브라 객체
	class ASombraHero* SombraPlayer;
	
	//솜브라 스킬 입력
	class UInputAction* IA_EMP;
	class UInputAction* IA_Hack;
	class UInputAction* IA_HackOff;
	class UInputAction* IA_Virus;
	class UInputAction* IA_Translocator;

	//솜브라 스킬 입력 handle
	uint32 Handle_IA_EMP;
	uint32 Handle_IA_Hack;
	uint32 Handle_IA_HackOff;
	uint32 Handle_IA_Virus;
	uint32 Handle_IA_Translocator;
	
	//솜브라 Translocator
	//발사 속력
	float ProjectileLaunchSpeed = 3600.f;
	//공중 유지 시간
	float ProjectileMaxFlyingTime = 0.75f;
	//발사체 원본
	UPROPERTY()
	TSubclassOf<class ATranslocatorProjectile> OriginTranslocatorProjectile; 
	
protected:
public:
	//=====인터페이스 함수 구현=====
protected:
	virtual void SetupHeroInputInfo(class UEnhancedInputComponent* enhancedInputComponent) override;
	//=====솜브라 스킬 입력 반응 함수=====
private:
	//솜브라 스킬에 반응하는 함수
	void OnEMP(const struct FInputActionValue& value);
	void OnHack(const struct FInputActionValue& value);
	void OnHackCancled(const struct FInputActionValue& value);
	void OnVirus(const struct FInputActionValue& value);
	void OnTranslocator(const struct FInputActionValue& value);

	UFUNCTION(Server, Reliable)
	void ServerRPC_OnTranslocator(const struct FInputActionValue& value);
	

	//=====부모 함수 오버라이드=====/
public:
	virtual void TakeDamage() override;
	
	//=====위치 변환기=====
private:
	//이동 시간
	float MoveTime = 0.75f;
public:
	//솜브라 스킬 외부 트리거
	void TriggerTranslocator(FVector end);

	//=====은신=====
private:
	//은신 로직 핸들 - 은신 여부 확인 용 - 쿨 타이머 컴포넌트의 영향을 받음.
	FTimerHandle CoolTimer_StealthTimerHandle;
	//은신 감지 상태 비트마스크
	uint32 DetectionLayer;
	//은신 지속 시간
	float StealthTime = 5.f;
	//은신 적 감지 반경
	float DetectionRadius = 400.f;
	//hit 은신 감지 지속 시간
	float HitDetectionTime = 1.3f;
	//hit 은신 감지 핸들
	FTimerHandle HitDetectionTimerHandle;

	//은신 감지 상태를 변화시킨다. switch가 true일 경우 켜고, false라면 끈다.
	void SetDetectionLayer(EDetection newDetection, bool bSwitch);
	//은신을 시작한다.
	void StartStealth();
	//은신을 종료시킨다.
	void EndStealth();
	//은신 지속 중 할 일
	void StealthTick(float deltaTime, float currentTime);
	//은신 시간 종료 시 notify 함수
	void NotifyStealthEnd(float excessDeltaTime);

	//=====해킹=====
private:
	//해킹 타겟 감지용 정보
	UPROPERTY()
	FHackTargetCalInfo HackTargetCalInfo;
	//해킹 타겟.
	AHeroBase* HackTarget;
	//이전 해킹 타겟.
	AHeroBase* BeforeHackTarget;
	//해킹 시도 시간
	float CurrentHackTryTime;
	float MaxHackTryTime = 0.65f;
	
	
	//해킹 타겟 감지
	bool DetectHackTarget(class AHeroBase*& outHeroBase);
	//해킹 용 ConeTrace 기본 값 계산 함수
	void CalHackConeTrace();
	//해킹 용 ConeTrace 중 두번 째 Trace에 대하여, 콘의 꼭짓점이 되는 좌표 계산
	void CalSecondHackTraceBaseStartPoint();
	//해킹 용 ConeTrace 중 첫번 째 Trace에 대하여, 가장 가까운 대상을 감지
	bool DetectHackTargetInFirstHackTrace(AHeroBase*& outHeroBase);
	//해킹 용 ConeTrace 중 두번 째 Trace에 대하여, 가장 가까운 대상을 감지
	bool DetectHackTargetInSecondHackTrace(AHeroBase*& outHeroBase);
	//해킹 감지 관련 tick 함수.
	void HackTick(float deltaTime);

	
// //Test============================
// 	
// 	void A()
// 	{
// 		//스텔스 상태 변환
// 		SombraPlayer->EnterStealth();
// 	}
//
// 	void BTick(float deltaTimes, float currentTimes);

	//=====스킬 아이콘=====//
private:
	//스킬 아이콘 인덱스
	int32 TranslocatorIconIndex;
	//스킬 아이콘 원본 텍스쳐
	class UTexture2D* OriginTranslocatorTexture2D;

	//==스킬 쿨타임==//
private:
	bool bTranslocator = true;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float TranslocatorCoolTime = 6.f;
};
