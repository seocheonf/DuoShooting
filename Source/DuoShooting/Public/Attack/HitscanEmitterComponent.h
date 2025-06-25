// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI/ShootingMainWidget.h"
#include "HitscanEmitterComponent.generated.h"

/* 카메라 방향으로 히트스캔형 총을 연사하는 컴포넌트
 * 연사 간격, 최대거리, 한발당 공격력 등 설정가능
 */

UENUM(BlueprintType)
enum class EHitscanEmitterState : uint8
{
	IDLE UMETA(DisplayName = "기본"),
	TRIGGERED UMETA(DisplayName = "사격 눌림 상태"),
	RELOADING UMETA(DisplayName = "재장전 상태"),
	BLOCKED UMETA(DisplayName = "사용 불가")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUOSHOOTING_API UHitscanEmitterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHitscanEmitterComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	/// ==========공통==========
private:
	EHitscanEmitterState State = EHitscanEmitterState::IDLE;
	void SetState(EHitscanEmitterState newState);

	UPROPERTY()
	class AHeroBase* Owner;
	
	UPROPERTY()
	class UCameraComponent* OwnerCamera;

	UPROPERTY()
	class UShootingMainWidget* ShootingMainWidget;

	// 현재 총알 개수 * MaxBullet 정보는 Hero가 가지고 있다
	UPROPERTY(EditAnywhere)
	int32 CurrentBullet = 100;
	void SetCurrentBullet(int32 bullets);

public:
	// 액터에서 필요한 인스턴스 전달받음
	void Initialize(UShootingMainWidget* mainWidgetInst, UCameraShakeSourceComponent* camShakeSourceInst);

	// 인풋 전달
	void SetupHitscanInputInfo(UEnhancedInputComponent* enhancedInputComponent);
	
	/// ==========총기==========
private:
	void InputFire_Started();
	void InputFire_Completed();

	void TickHitScan(float dt);
	void SingleLineTrace();

	// 네트워크
	UFUNCTION(Server, Reliable)
	void ServerRPC_InputFireStarted();
	UFUNCTION(Server, Reliable)
	void ServerRPC_RequestSingleLineTrace();
	UFUNCTION(Client, Reliable)
	void ClientRPC_ReceiveSingleLineTraceResult(int bulletCount);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_ReceiveSingleLineTraceResult(FVector hitLocation);	
	UFUNCTION(Server, Reliable)
	void ServerRPC_InputFireCompleted();
	
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_Fire;

	// 연사 간격
	float FireInterval = 0.025f;
	float FireTimer = 1000.0f; // 충분히 큰 수
	// 최대 거리
	float MaxDistance = 10000.0f;
	// 한발당 공격력
	float DamagePerBullet = 5.0f;
	// 방향 정확도 (0일 떄 완전히 정확함 숫자가 커질수록 난사)
	float Spread = 2.0f;

	// 이펙트
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* FireParticle = nullptr;
	UPROPERTY()
	class UCameraShakeSourceComponent* CameraShakeSourceComp;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UCameraShakeBase> FireCameraShake;

	//==김형모
	//총기 사운드 데이터
	class USoundBase* OriginSoundShoot;
	//총기 사운드 Attenation
	class USoundAttenuation* OriginSoundAttenuation;
	//총기 장전 사운드 데이터
	class USoundBase* OriginSoundReload;
	
	//총기 사운드 발생 요청
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_PlaySoundShoot();

	//총기 장전 사운드 발생 요청
	UFUNCTION(Client, Reliable)
	void ClientRPC_PlaySoundReload();

public:
	void SetHitScanSettings(float fireInterval, float damagePerBullet, float spread, float maxDist = 10000.0f);
	
	/// ==========리로드==========
private:
	void InputReload();

	void StartReload();
	void EndReload();
	void Server_EndReloading();

	UFUNCTION(Server, Reliable)
	void ServerRPC_Reload();
	UFUNCTION(Client, Reliable)
	void ClientRPC_ReloadEnd(int bulletCount);
	
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_Reload;
	
	/// ==========이 컴포넌트 활성화/비활성화==========
private:
	UFUNCTION(Server, Reliable)
	void ServerRPC_Enable();
	UFUNCTION(Server, Reliable)
	void ServerRPC_Disable();

public:
	void Enable();
	void Disable();
};
