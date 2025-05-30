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

	//=====변수=====
private:
	// 은신 여부
	bool bStealth;
	// 은신 세부 상태
	EStealthState StealthState;
	// 보여짐 전환 보간용 타이머
	FTimerHandle VisibilityTimerHandle;

	// base material
	UMaterial* OriginSombraMaterial;
	// 보여짐 전환 시 material 값 변경용
	UMaterialInstanceDynamic* SombraMaterialInstance;
	UMaterialInstanceDynamic* SombraMaterialInstance2;
	// 현재 material alpha 값
	float SombraMaterialAlpha = 1.0f;
	// 현재 alpha 변화 진행 정도
	float CurrentAlphaTime = 0.f;
	// alpha 변화 진행 정도 마무리
	float MaxAlphaTime = 0.25f;
	
protected:
public:
	//=====함수=====
protected:
	//material alpha값 변환 (타이머 보간, 타이머 단일 실행)
	void SetVisibilityAlpha(float alpha);
public:
	//모습과 충돌 드러냄 (모습은 현재 은신 여부에 영향을 받음)
	void SetAppearance();
	//모습과 충돌 숨김
	void SetDisAppearance();
	//은신 진입 (은신 세부 상태 Hidden)
	void EnterStealth();
	//은신 탈출 (은신 세부 상태 None)
	void ExitStealth();
	//은신 세부 상태 변환
	void SetStealthState(EStealthState newState);
	
	//==고유 함수 영역==
private:
protected:
public:
};
