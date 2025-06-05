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
	UPROPERTY(Replicated)
	bool bEnabled = true;
	
	// 사격이 트리거되었는지 (트리거되어있으면 연사)
	UPROPERTY(Replicated)
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
	
	void TickHitScan(float dt);

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
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* FireParticle = nullptr;

	UPROPERTY()
	class UCameraShakeSourceComponent* CameraShakeSourceComp;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UCameraShakeBase> FireCameraShake;
	
	// ----------UI----------
	UPROPERTY()
	class UShootingMainWidget* ShootingMainWidget;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void DebugInfo();
	
public:
	// 인풋 전달
	void SetupHitscanInputInfo(UEnhancedInputComponent* enhancedInputComponent);

	// 액터에서 필요한 인스턴스 전달받음
	void Initialize(UShootingMainWidget* mainWidgetInst, UCameraShakeSourceComponent* camShakeSourceInst);

	void SetHitScanSettings(float fireInterval, float damagePerBullet, float spread, float maxDist);
	
	// 사격 활성화/비활성화
	void Enable();
	void Disable();

	// 네트워크
	void FireNetwork_Shooter(int bulletCount);						// 총을 쏜 사람에게만 보이는 이펙트
	void FireNetwork_Everyone(FVector hitLocation);	// 총을 쏘았을 때 모두에게 보일 이펙트
};
