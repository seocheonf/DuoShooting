// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PulseBomb.generated.h"

enum class EPulseBombState : uint8
{
	INACTIVE,
	FLYING,
	ATTACHING,
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
	// 콜라이더
	UPROPERTY(VisibleDefaultsOnly)
	class USphereComponent* SphereComp;

	// 메쉬
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComp;

	// 투사체
	UPROPERTY(VisibleDefaultsOnly)
	class UProjectileMovementComponent* ProjectileMovementComp;

	// 폭탄의 현재 상태
	EPulseBombState CurrentState = EPulseBombState::INACTIVE;
	
	// 최대/최소 데미지
	UPROPERTY(EditDefaultsOnly)
	float MaximumDamage = 350.0f;
	UPROPERTY(EditDefaultsOnly)
	float MinimumDamage = 70.0f;

	// 폭발 반경
	UPROPERTY(EditDefaultsOnly)
	float Radius = 400.0f;

	// 최대 데미지를 입는 반경 (Radius보다는 작아야 한다)
	UPROPERTY(EditDefaultsOnly)
	float Radius_FullDamage = 200.0f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY()
	AController* InstigatorController;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* hitComponent, AActor* otherActor, UPrimitiveComponent* otherComp,
	                   FVector normalImpulse, const FHitResult& hit);

	void Explode();
	
	FTimerHandle ExplosionTimerHandle;
	
public:
	// 던지기
	void Launch(FVector direction, float speed, AController* instigator);
};
