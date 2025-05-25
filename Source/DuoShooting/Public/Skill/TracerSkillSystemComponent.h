// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillSystemComponent.h"
#include "TracerSkillSystemComponent.generated.h"


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
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_Blink;
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* IA_Recall;
protected:
public:
	//=====함수=====
protected:
	virtual void SetupHeroInputInfo(class UEnhancedInputComponent* enhancedInputComponent) override;
public:
	//==고유 함수 영역==
private:
	void InputBlink(const struct FInputActionValue& value);
	void InputRecall(const struct FInputActionValue& value);
protected:
public:
};
