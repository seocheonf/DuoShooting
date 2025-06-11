// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SombraAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/SombraHero.h"

void USombraAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	SombraHero = Cast<ASombraHero>(TryGetPawnOwner());
	if (nullptr == SombraHero)
		UE_LOG(LogTemp, Error, TEXT("SombraAnimInstance : SombraHero is null"));
		//SombraHero = Cast<ASombraHero>(GetWorld()->GetFirstPlayerController()->GetPawn());

	
}

void USombraAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (nullptr == SombraHero)
		return;

	IsInAir = SombraHero->GetMovementComponent()->IsFalling();

	Speed = SombraHero->GetVelocity().Length();

	Pitch = SombraHero->GetBaseAimRotation().Pitch;
	Pitch = Pitch > 180 ? Pitch - 360.0f : Pitch;
	
	IsAccelerating = SombraHero->GetCharacterMovement()->GetCurrentAcceleration().Length() > 0;

	FullBody = GetCurveValue(FName("FullBody")) > 0;
	
}

void USombraAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage);
}
