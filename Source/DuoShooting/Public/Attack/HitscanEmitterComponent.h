// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HitscanEmitterComponent.generated.h"

/* 카메라 방향으로 히트스캔을 연사하는 컴포넌트
 * StartTrigger()로 연사 시작, EndTrigger()로 연사 정지
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

	bool bTriggered = false;
	
	// 연사 간격
	float HitScanInterval = 0.025f;
	float FireTimer = 1000.0f; // 충분히 큰 수

	// 최대 거리
	float HitScanDistance = 10000.0f;

	// 한발당 공격력
	float DamageAmount = 5.0f;

	// 방향 정확도 (0일 떄 완전히 정확함 숫자가 커질수록 난사)
	float Spread = 0.0f;

	// 하나 쏘기
	void SingleLineTrace();
	
public:
	// 연사 시작
	void StartTrigger();

	// 연사 정지
	void EndTrigger();
};
