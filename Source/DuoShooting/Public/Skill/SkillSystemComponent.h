// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillSystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUOSHOOTING_API USkillSystemComponent : public UActorComponent
{
	GENERATED_BODY()

	//=====재 정의 함수=====
public:
	// Sets default values for this component's properties
	USkillSystemComponent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// 초기화 함수.
	virtual void InitializeComponent() override;

	//============쿨타임 계산 매니저===========//

	//===========스킬 공통 정보===========/
private:
	//스킬 사용 가능 여부
	UPROPERTY()
	bool bEnableSkill = true;
protected:
	//스킬 시스템과 종속적인 입력 처리 모음
	UPROPERTY()
	class UInputMappingContext* IMC_SkillSystem;
	//스킬 시스템이 부착된 플레이어 대상
	UPROPERTY()
	class AHeroBase* TargetPlayer; //나중에 get만.
	
	//플레이어 입력
	class UEnhancedInputComponent* TargetPlayerEnhancedInputComponent;
	
	//============스킬 컴포넌트 사용 준비============/
protected:
	//스킬 컴포넌트와 영웅의 입력을 바인딩합니다. 스킬과 영웅의 특징에 맞게 바인딩 해주세요!
	virtual void SetupHeroInputInfo(class UEnhancedInputComponent* enhancedInputComponent) PURE_VIRTUAL(USkillSystemComponent::SetupPlayerInfo, );
public:
	//스킬 컴포넌트에 영웅을 등록하고, 스킬 컴포넌트와 영웅의 입력을 바인딩하는 함수를 수행합니다.  
	void SetupHeroInfo(class AHeroBase* targetPlayer, class UEnhancedInputComponent* enhancedInputComponent);

	//타겟 플레이어의 InputComponent에 입력을 재 등록합니다.
	void ReSetupHeroInputInfo();
	//스킬 컴포넌트와 영웅의 입력을 언바인딩합니다.
	void RemoveHeroInputInfo();


	//============궁극기 시스템============//
private:
	//현재 궁극기 게이지
	int32 CurrentUltimateSkillGauge = 0;
	//궁극기 소모 게이지
	int32 MaxUltimateSkillGauge = MAX_int32;
	//궁극기 시전 여부
	bool bUltimateSkillCasting = false;
	//궁극기 지속 여부
	bool bUltimateSkillMaintaining = false;
	
protected:
	//스킬 게이지 Get, Set 함수
	void SetCurrentUltimateSkillGauge(int32 skillGauge);
	int32 GetCurrentUltimateSkillGauge();
	//현재 궁극기 게이지를 백분율(퍼센트)로 반환
	int32 GetUltimateSkillGaugePercent();
public:
	//궁극기 게이지 충전. 일부 상황(궁극기 데미지, 궁극기 사용중 등)에선 충전되지 않음.
	void ChargeUltimateGauge(int32 amount);


	//============변수 받아오기=============//
public:
	AActor* GetTargetActor();

	//============피격 정보 받아오기==========//
public:
	virtual void TakeDamage();

	//==스킬 UI 관련==//
private:
	//스킬 UI
	class USkillSystemBaseUI* SkillUI;
	//스킬 UI 기본 정보
	TSubclassOf<class USkillSystemBaseUI> OriginSkillUI;
	//스킬 갯수
	int32 NextSkillIndex = 0;
protected:
	//스킬 UI에 추가
	int32 AddSkillUI(UTexture2D* skillIcon, FText skillKeyName);
	//스킬 쿨타임 UI 적용. 클라이언트에서만 해야 할 일
	UFUNCTION(Client, Reliable)
	void ClientRPC_SetSkillCoolTimeUI(int32 index, float upper, float lower);

	//스킬 아이콘 활성, 비활성 요청 RPC함수.
	UFUNCTION(Client, Reliable)
	void ClientRPC_SetSkillIconActivation(int index, bool bActive, bool bForbidden = false);

	//스킬 쿨타임 숫자 표시 요청 RPC함수.
	UFUNCTION(Client, Reliable)
	void ClientRPC_SetSkillRemainTimeUI(int32 index, int32 remainTime, bool bEmpty);

protected:
	virtual void DoAfterTargetPlayerDie();

	//==스킬 UI==
public:
	void RemoveSkillUI();
	void SetSkillUI(USkillSystemBaseUI* skillUI);
	void AddAdditionalSkillUI(int32 index, UUserWidget* widget);

	//==스킬 사운드==
private:
	//스킬 온 사운드
	class USoundBase* OriginSoundSkillOn;

protected:
	//스킬 온 사운드 재생
	UFUNCTION(Client, Reliable)
	void ClientRPC_PlaySoundSkillOn();
};
