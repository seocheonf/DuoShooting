// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/TracerAnimInstance.h"

#include "Player/TracerHero.h"

void UTracerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	tracer = Cast<ATracerHero>(TryGetPawnOwner());
}

void UTracerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (tracer)
	{
		FVector vel = tracer->GetVelocity();

		// 캐릭터의 속도를 가져와서 부드럽게 스피드와 방향 계산
		float rawSpeed = FMath::GetMappedRangeValueClamped(FVector2D(-1.0f, 1.0f), FVector2D(0.0f, 100.0f),
		                                          FVector::DotProduct(vel, tracer->GetActorForwardVector()));
		
		float rawDir = FMath::GetMappedRangeValueClamped(FVector2D(-1.0f, 1.0f), FVector2D(0.0f, 100.0f),
										  FVector::DotProduct(vel, tracer->GetActorRightVector()));

		Speed = FMath::FInterpTo(Speed, rawSpeed, DeltaSeconds, LegsBlendSpaceInterpSpeed);
		Direction = FMath::FInterpTo(Direction, rawDir, DeltaSeconds, LegsBlendSpaceInterpSpeed);

		PitchAngle = -tracer->GetBaseAimRotation().GetNormalized().Pitch;
		PitchAngle = FMath::Clamp(PitchAngle, -60.0f, 60.0f);
	}
}
