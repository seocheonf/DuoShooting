// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillSystemComponent.h"
#include "Tool/FixedDeque.h"
#include "TracerSkillSystemComponent.generated.h"

UENUM(BlueprintType)
enum class ETracerSkillState : uint8
{
	NONE UMETA(DisplayName = "없음"),
	BLINK UMETA(DisplayName = "점멸"),
	RECALL UMETA(DisplayName = "시간 역행")
};

// 트레이서가 시간역행을 위해 일정 주기로 기록할 정보 구조체
USTRUCT(BlueprintType)
struct FTracerRecallInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	float Health;

	UPROPERTY()
	FVector2D ControlRotation;

	FTracerRecallInfo();
	FTracerRecallInfo(const FVector& location, float controlRot_Pitch, float controlRot_Yaw, float health);
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUOSHOOTING_API UTracerSkillSystemComponent : public USkillSystemComponent
{
	GENERATED_BODY()

	//=====재 정의 함수=====
public:
	// Sets default values for this component's properties
	UTracerSkillSystemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	//=====변수=====
private:
	UPROPERTY()
	class ATracerHero* Owner;
	// Input
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_PulseBomb;
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_Blink;
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_Recall;
	// 펄스 폭탄(궁극기) 관련
	UPROPERTY(EditAnywhere)
	TSubclassOf<class APulseBomb> PulseBombFactory;
	// 현재 상태
	ETracerSkillState CurrentSkillState;
	// 점멸 관련
	UPROPERTY(EditDefaultsOnly)
	float BlinkDuration = 0.1f; // 걸리는 시간
	UPROPERTY(EditDefaultsOnly)
	int32 BlinkSpeed = 6500; // 점멸 속도
	UPROPERTY(VisibleAnywhere)
	FTimerHandle BlinkTimerHandle;
	FVector BlinkStartPos; // 점멸 시작시의 시작방향 저장 (보다 정확한 동기화를 위해)
	FVector BlinkDirection; // 점멸 방향
	//FVector TestStartLocation;
	// 시간 역행 관련
	// 리콜할 레코드(기록) 정보
	UPROPERTY(EditDefaultsOnly)
	float RecordInterval = 0.1f; // 몇초마다 기록할것인지
	UPROPERTY(EditDefaultsOnly)
	int32 RecordLength = 30; // 몇개까지 기록할 것인지
	FixedDeque<FTracerRecallInfo> Records; // 기록 컨테이너
	UPROPERTY(EditDefaultsOnly)
	float RecallInterval = 0.91f; // 몇초만에 역행할것인지?
	UPROPERTY(VisibleAnywhere)
	FTimerHandle RecallTimerHandle;
	// 시간역행 보간용 변수들
	float TimeSinceLastRecallInterval;
	FTracerRecallInfo IntervalOrigin;
	FTracerRecallInfo IntervalTarget;
	float RecallStepDuration;

protected:
public:
	//=====함수=====
protected:
	virtual void SetupHeroInputInfo(class UEnhancedInputComponent* enhancedInputComponent) override;

public:
	//==고유 함수 영역==
private:
	// Input
	void InputBlink(const struct FInputActionValue& value);
	void InputRecall(const struct FInputActionValue& value);
	void InputPulseBomb(const struct FInputActionValue& value);
	// 점멸 관련
	void TickBlink();
	void DeactivateBlink();
	// 시간 역행 관련
	void ActivateRecall();
	void TickRecall(float DeltaTime);
	void DeactivateRecall();
	void RecordInfo();
	void RecallInfo();
	void ToggleRecallOwnerSettings(bool isRecall);

	void DebugInfo();

protected:
public:
	ETracerSkillState GetCurrentSkillState() const;
	UFUNCTION(Server, Reliable)
	void ServerRPC_BlinkStart(FVector StartPos, FVector Direction);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_BlinkStart(FVector StartPos, FVector Direction);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_BlinkEnd();
	UFUNCTION(Server, Reliable)
	void ServerRPC_RecallStart();
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_RecallStart();
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_RecallEnd();
	UFUNCTION(Server, Reliable)
	void ServerRPC_ThrowPulseBomb();
	// UFUNCTION(Client, Reliable)
	// void ClientRPC_FireHitScan(int bulletCount);
	// UFUNCTION(NetMulticast, Reliable)
	// void MultiRPC_FireEffects(FVector hitLocation);



	//====김형모====
private:
	//스킬 아이콘 인덱스
	int32 BlinkIconIndex;
	int32 RecallIconIndex;
	//스킬 아이콘 원본 텍스쳐
	class UTexture2D* OriginBlinkTexture2D;
	class UTexture2D* OriginRecallTexture2D;
private:
	//==스킬 쿨타임==
	bool bBlink = true;
	bool bRecall = true;
	bool bPulseBomb = true;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float BlinkCoolTime = 3.f;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float RecallCoolTime = 12.f;
};
