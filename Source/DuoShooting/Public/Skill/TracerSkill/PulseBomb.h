// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PulseBomb.generated.h"

enum class EPulseBombState : uint8
{
	INACTIVE,
	FLYING,
	ATTACHED,
	EXPLODING
};

UCLASS()
class DUOSHOOTING_API APulseBomb : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APulseBomb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	EPulseBombState CurrentState = EPulseBombState::INACTIVE;

	// 콜라이더
	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* BoxComp;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComp;

	//이동 처리
	UPROPERTY(VisibleDefaultsOnly)
	class UProjectileMovementComponent* ProjectileMovementComp;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComp,
	                   FVector normalImpulse, const FHitResult& hit);

public:
	// 던지기
	void Launch(FVector direction, float speed);
};
