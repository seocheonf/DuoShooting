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
	float LegsBlendSpaceInterpSpeed = 15.0f;
	
protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UPROPERTY()
	class ATracerHero* tracer;
	
public:
	UPROPERTY(BlueprintReadWrite)
	float Direction;

	UPROPERTY(BlueprintReadWrite)
	float Speed;

	UPROPERTY(BlueprintReadWrite)
	float PitchAngle;
};
