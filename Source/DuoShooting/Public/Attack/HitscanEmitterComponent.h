// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI/ShootingMainWidget.h"
#include "HitscanEmitterComponent.generated.h"

/* 카메라 방향으로 히트스캔형 총을 연사하는 컴포넌트
 * 연사 간격, 최대거리, 한발당 공격력 등 설정가능
 */

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUOSHOOTING_API UHitscanEmitterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHitscanEmitterComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	class AHeroBase* Owner;
	
	UPROPERTY()
	class UCameraComponent* OwnerCamera;

	/// ----------인풋----------
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_Fire;
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_Reload;
	
	// 연사 시작/정지
	void InputFire_Started();
	void InputFire_Completed();

	// 리로드
	void InputReload();
	
	// 사격 가능한지 (인풋 막기용)
	bool bEnabled = true;
	
	// 사격이 트리거되었는지 (트리거되어있으면 연사)
	bool bTriggered = false;

	/// ----------총알----------
	// 현재 총알 개수
	UPROPERTY(EditAnywhere)
	int32 CurrentBullet = 100;

	void SetCurrentBullet(int32 bullets);

	// 한발 쏘기
	void SingleLineTrace();

	// 재장전
	void Reload();

	/// ----------세부설정----------
	// 연사 간격
	float FireInterval = 0.025f;
	float FireTimer = 1000.0f; // 충분히 큰 수

	// 최대 거리
	float MaxDistance = 10000.0f;

	// 한발당 공격력
	float DamagePerBullet = 5.0f;

	// 방향 정확도 (0일 떄 완전히 정확함 숫자가 커질수록 난사)
	float Spread = 2.0f;

	// ----------이펙트----------
	UPROPERTY()
	class UParticleSystem* FireParticle = nullptr;

	// ----------UI----------
	UPROPERTY()
	class UShootingMainWidget* ShootingMainWidget;
	
public:
	// 인풋 전달
	void SetupHitscanInputInfo(UEnhancedInputComponent* enhancedInputComponent);

	// 메인위젯 인스턴스 전달
	void SetShootingMainWidget(UShootingMainWidget* inst);

	void SetHitScanSettings(float fireInterval, float damagePerBullet, float spread, float maxDist);
	
	// 사격 활성화/비활성화
	void Enable();
	void Disable();
};
