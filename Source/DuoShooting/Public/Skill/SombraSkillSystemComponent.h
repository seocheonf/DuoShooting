// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillSystemComponent.h"
#include "SombraSkillSystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUOSHOOTING_API USombraSkillSystemComponent : public USkillSystemComponent
{
	GENERATED_BODY()

	//=====재 정의 함수=====
public:
	// Sets default values for this component's properties
	USombraSkillSystemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	//=====변수=====
private:
	//솜브라 스킬 입력
	class UInputAction* IA_EMP;
	class UInputAction* IA_Hack;
	class UInputAction* IA_Virus;
	class UInputAction* IA_Translocator;
protected:
public:
	//=====함수=====
protected:
	virtual void SetupHeroInputInfo(class UEnhancedInputComponent* enhancedInputComponent) override;
	//==고유 함수 영역==
private:
	//솜브라 스킬에 반응하는 함수
	void OnEMP(const struct FInputActionValue& value);
	void OnHack(const struct FInputActionValue& value);
	void OnVirus(const struct FInputActionValue& value);
	void OnTranslocator(const struct FInputActionValue& value);
protected:
public:
};
