// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillSystemComponent.h"
#include "Player/SombraHero.h"
#include "SombraSkillSystemComponent.generated.h"

UENUM()
enum class EDetection
{
	PlayerDetection = 0,
	HitDetection = 1,
	HackDetection = 2
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
	class UInputAction* IA_Virus;
	class UInputAction* IA_Translocator;

	//솜브라 Translocator
	//발사 속력
	float ProjectileLaunchSpeed = 7200.f;
	//공중 유지 시간
	float ProjectileMaxFlyingTime = 0.25f;
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
	void OnVirus(const struct FInputActionValue& value);
	void OnTranslocator(const struct FInputActionValue& value);

	//=====위치 변환기=====
private:
	//이동 시간
	float MoveTime = 0.25f;
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



	
// //Test============================
// 	
// 	void A()
// 	{
// 		//스텔스 상태 변환
// 		SombraPlayer->EnterStealth();
// 	}
//
// 	void BTick(float deltaTimes, float currentTimes);

	
};
