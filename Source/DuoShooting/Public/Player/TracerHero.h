// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HeroBase.h"
#include "TracerHero.generated.h"

class UTracerAnimInstance;

UCLASS()
class DUOSHOOTING_API ATracerHero : public AHeroBase
{
	GENERATED_BODY()

	//=====재 정의 함수=====
public:
	// Sets default values for this character's properties
	ATracerHero();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void InputMove(const struct FInputActionValue& value) override;
	virtual void InputLook(const struct FInputActionValue& value) override;
	virtual void InputJump(const struct FInputActionValue& value) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//=====변수=====
private:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* FirstViewSkeletalMeshComp;
	UPROPERTY()
	class UTracerSkillSystemComponent* TracerSkillSystemComp;
	UPROPERTY()
	class UTracerAnimInstance* TracerAnimInstance;
protected:
	// 점멸 이펙트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UNiagaraComponent* BlinkNiagaraComponent;
	// 시간 역행 이펙트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UNiagaraComponent* RecallNiagaraComponent;
protected:
	// 아래를 바라볼 때 일인칭 메쉬가 얼마나 뒤로 갈 것인지
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float FirstViewSkeletalMeshCompLookDownRetractScalar = 110.0f;
public:
	//=====함수=====
private:
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_PlayFireMontage();
protected:
	virtual void DoAfterAction(EHeroActionType actionType) override;
	virtual void DieAfterAction() override;
public:
	//==고유 함수 영역==
private:
protected:
public:
	class UNiagaraComponent* GetRecallNiagaraComponent() const { return RecallNiagaraComponent; }
	class UNiagaraComponent* GetBlinkNiagaraComponent() const { return BlinkNiagaraComponent; }
};
