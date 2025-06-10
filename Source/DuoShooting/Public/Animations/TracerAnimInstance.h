// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TracerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API UTracerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	float LegsBlendSpaceInterpSpeed = 16.0f;
	float LandingDistanceFromGround = 100.0f;
	
protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UPROPERTY()
	class ATracerHero* tracer;
	
public:
	UPROPERTY(BlueprintReadOnly)
	float Direction;

	UPROPERTY(BlueprintReadOnly)
	float Speed;

	UPROPERTY(BlueprintReadOnly)
	float PitchAngle;

	UPROPERTY(BlueprintReadOnly)
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly)
	float Velocity_Z;

	UPROPERTY(BlueprintReadOnly)
	bool ShouldLand;
};
