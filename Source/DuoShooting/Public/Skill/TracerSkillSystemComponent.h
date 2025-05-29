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

USTRUCT(BlueprintType)
struct FTransformSnapshot
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	FTransformSnapshot();
	FTransformSnapshot(const FVector& Location, const FRotator& Rotation);
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
	class UInputAction* IA_Blink;
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_Recall;
	// 현재 상태
	ETracerSkillState CurrentSkillState;
	// 점멸 관련
	UPROPERTY(EditDefaultsOnly)
	float BlinkDuration = 0.05f;	// 걸리는 시간
	UPROPERTY(EditDefaultsOnly)
	int32 BlinkSpeed = 20000;		// 점멸 속도
	UPROPERTY(VisibleAnywhere)
	FTimerHandle BlinkTimerHandle;
	// 시간 역행 관련
	// 리콜할 레코드(기록) 정보
	UPROPERTY(EditDefaultsOnly)
	float RecordInterval = 0.05f;	// 얼마나 자주 기록할것인지
	UPROPERTY(EditDefaultsOnly)
	int32 RecordLength = 100;		// 몇개까지 기록할 것인지
	FixedDeque<FTransformSnapshot> Records; // 기록 컨테이너
	UPROPERTY(VisibleAnywhere)
	FTimerHandle RecallTimerHandle;
	UPROPERTY(EditDefaultsOnly)
	float RecallInterval = 2.0f;	// 몇초만에 역행할것인지?

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
	// 점멸 관련
	void ActivateBlink();
	void TickBlink();
	void DeactivateBlink();
	// 시간 역행 관련
	void RecordPoints();
	void RecallPoints();
	void ActivateRecall();
	void DeactivateRecall();
protected:
public:
	ETracerSkillState GetCurrentSkillState() const;
};
