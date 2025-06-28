// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TranslocatorProjectile.generated.h"

UCLASS()
class DUOSHOOTING_API ATranslocatorProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATranslocatorProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//=====변수=====
private:
	//==기본==
	//충돌 체크
	UPROPERTY(VisibleDefaultsOnly)
	class USphereComponent* SphereComp;
	//모습
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* StaticMeshComp;
	//이동 처리
	UPROPERTY(VisibleDefaultsOnly)
	class UProjectileMovementComponent* MovementComp;

	//모습용 메쉬
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMesh* OriginStaticMesh;
	//모습용 머티리얼
	UPROPERTY(VisibleDefaultsOnly)
	class UMaterial* OriginMaterial;

	//==투사체==
	//발사체(특정 상황 시 정보를 전달해야 하는 목표이자 대상)
	class USombraSkillSystemComponent* Launcher;
	// //투사체 방향
	// FVector LaunchDirection;
	// //투사체 속도
	// float LaunchSpeed;
	//투사체 공중 최대 유지 시간
	float MaxFlyingTime;
	//투사체 공중 현재 유지 시간
	float CurrentFlyingTime;

	//==시간 만료로 인한 종료 체크==//
	bool bStop = false;

	//operate를 했는지 여부. 한번 operate하면, 더이상 operate가 되지 말아야 함. 그 친구의 역할은 한번 하고 끝나니까.  
	bool bOperation = false;

	//현재 속도를 클라이언트에 동기화
	UPROPERTY(ReplicatedUsing=OnRep_SetVelocity)
	FVector CurrentVelocity;
	//서버상 현재 속도를 클라이언트상 속도에 적용
	UFUNCTION()
	void OnRep_SetVelocity();
	
	//==초기화==
#if WITH_EDITOR
	bool bInitial;
#endif
	//=====함수=====
private:
	//생성자 시점 정보 초기화
	void ConstructorInit();
	//BeginPlay2
	void CustomBeginPlay();
	//충돌 함수
	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	//조건 만족(충돌, 시간)시 호출되어 발사자(솜브라 스킬 시스템 컴포넌트)에 상황 전달
	void OnOperate();
public:
	void Initializer(class USombraSkillSystemComponent* launcher, const FVector& launchPoint, const FVector& launchDirection, const float launchSpeed, const float flyingTime);
	
};