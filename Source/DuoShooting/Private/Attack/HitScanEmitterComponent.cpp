// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack/HitScanEmitterComponent.h"


// Sets default values for this component's properties
UHitScanEmitterComponent::UHitScanEmitterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHitScanEmitterComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	//StartPosition = GetOwner()
	
}


// Called every frame
void UHitScanEmitterComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHitScanEmitterComponent::Fire()
{
	// FHitResult OutHit;
	// FVector Start = GetFollowCamera()->GetComponentLocation();
	// FVector End = Start + GetFollowCamera()->GetForwardVector() * 10000.0f;
	// FCollisionQueryParams Params;
	// Params.AddIgnoredActor(this);
	//
	// bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_GameTraceChannel3, Params);
	//
	// if (bHit)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Hit"));
	// 	DrawDebugLine(GetWorld(), Start, OutHit.Location, FColor::Turquoise, false, 1.0f);
	//
	// 	if (auto saturnCharacter = Cast<ASaturnCharacter>(OutHit.GetActor()))
	// 	{
	// 		FPointDamageEvent DamageEvent;
	// 		DamageEvent.Damage = 1.0f;
	// 		DamageEvent.HitInfo = OutHit;
	//
	// 		AController* InstigatorController = GetController();
	// 		AActor* DamageCauser = this;
	//
	// 		UE_LOG(LogTemp, Warning, TEXT("SaturnCharacter Hit"));
	// 		saturnCharacter->TakeDamage(1.0f, DamageEvent, InstigatorController, DamageCauser);
	// 	}
	// }
}

