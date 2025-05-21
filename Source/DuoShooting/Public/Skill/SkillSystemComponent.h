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

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// 초기화 함수.
	virtual void InitializeComponent() override;
	
	//=====변수=====
private:
	//스킬 쿨타임
	UPROPERTY()
	float SkillCoolTime;
	//궁극기 게이지
	UPROPERTY()
	float UltimateGage;
	//스킬 사용 가능 여부
	UPROPERTY()
	bool bEnableSkill;
	//스킬 시스템이 부착된 플레이어 대상
	UPROPERTY()
	class AHeroBase* TargetPlayer;
	//스킬 시스템과 종속적인 입력 처리 모음
	UPROPERTY()
	class UInputMappingContext* IMC_SkillSystem;
	
protected:
public:
	//=====함수=====
private:
protected:
	//스킬 컴포넌트와 영웅의 입력을 바인딩합니다. 스킬과 영웅의 특징에 맞게 바인딩 해주세요!
	virtual void SetupHeroInputInfo(class UEnhancedInputComponent* enhancedInputComponent) PURE_VIRTUAL(USkillSystemComponent::SetupPlayerInfo, );
public:
	//스킬 컴포넌트에 영웅을 등록하고, 스킬 컴포넌트와 영웅의 입력을 바인딩하는 함수를 수행합니다.  
	void SetupHeroInfo(class AHeroBase* targetPlayer, class UEnhancedInputComponent* enhancedInputComponent);
};
