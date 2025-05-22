// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HitScanEmitterComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUOSHOOTING_API UHitScanEmitterComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UHitScanEmitterComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	// 연사 간격
	float HitScanInterval = 0.025f;

	// 최대 거리
	float HitScanDistance = 10000.0f;

	// 한발당 공격력
	float DamageAmount = 5.0f;
	
	void Fire();

	UPROPERTY()
	class USceneComponent* StartPosition;
	
public:
};
