// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/TracerAnimInstance.h"

#include "Player/TracerHero.h"
#include "GameFramework/CharacterMovementComponent.h"

UTracerAnimInstance::UTracerAnimInstance()
{
	// ConstructorHelpers::FObjectFinder<UAnimMontage> tempFireMontage(TEXT(
	// 	"'/Game/DuoShooting/Blueprints/Characters/Animation/Tracer/EditedDrongoAssets/Primary_Fire_Montage_Edited.Primary_Fire_Montage_Edited'"));
	// if (tempFireMontage.Succeeded()) FireMontage = tempFireMontage.Object;

	ConstructorHelpers::FObjectFinder<UAnimMontage> tempFireMontage(TEXT(
		"'/Game/LargeFile/ParagonDrongo/Characters/Heroes/Drongo/Animations/LevelStart_Montage.LevelStart_Montage'"));
	if (tempFireMontage.Succeeded()) FireMontage = tempFireMontage.Object;
}

void UTracerAnimInstance::PlayFireMontage()
{
	float test = -777.7f;
	if (FireMontage) test = Montage_Play(FireMontage);
	//else UE_LOG(LogTemp, Error, TEXT("PlayFireMontage failed"));
	
	UE_LOG(LogTemp, Log, TEXT("Montage Play Result: %f"), test);
}

void UTracerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Tracer = Cast<ATracerHero>(TryGetPawnOwner());
}

void UTracerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Tracer)
	{
		FVector vel = Tracer->GetVelocity();
		Velocity_Z = vel.Z;

		// 캐릭터의 속도를 가져와서 부드럽게 스피드와 방향 계산
		float rawSpeed = FMath::GetMappedRangeValueClamped(FVector2D(-1.0f, 1.0f), FVector2D(0.0f, 100.0f),
		                                                   FVector::DotProduct(vel, Tracer->GetActorForwardVector()));

		float rawDir = FMath::GetMappedRangeValueClamped(FVector2D(-1.0f, 1.0f), FVector2D(0.0f, 100.0f),
		                                                 FVector::DotProduct(vel, Tracer->GetActorRightVector()));
		
		Speed = FMath::FInterpTo(Speed, rawSpeed, DeltaSeconds, LegsBlendSpaceInterpSpeed);
		Direction = FMath::FInterpTo(Direction, rawDir, DeltaSeconds, LegsBlendSpaceInterpSpeed);

		// 피치 앵글 구하기
		PitchAngle = -Tracer->GetBaseAimRotation().GetNormalized().Pitch;
		PitchAngle = FMath::Clamp(PitchAngle, -60.0f, 60.0f);

		// 공중 여부
		bIsFalling = Tracer->GetCharacterMovement()->IsFalling();

		// 착지 시작 여부
		const FFindFloorResult& Floor = Tracer->GetCharacterMovement()->CurrentFloor;
		if (Floor.IsWalkableFloor()) ShouldLand = Floor.FloorDist < LandingDistanceFromGround;
		else ShouldLand = false;
	}
}
