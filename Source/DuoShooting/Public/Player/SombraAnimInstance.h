// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SombraAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API USombraAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	//==변수==//
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Speed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool IsInAir;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Pitch;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Yaw;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Roll;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FRotator RotationLastTick;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float YawDelta;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool IsAccelerating;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool IsAttacking;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int CurrentAttack;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool FullBody;

	class ASombraHero* SombraHero;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* ReloadMontage;
	
	//==함수==//
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	void PlayAttackMontage();
	void PlayReloadMontage();
};
