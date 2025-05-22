// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CoolTimerManagerComponent.generated.h"

DECLARE_DELEGATE_OneParam(FDoTimerTick, float)
DECLARE_DELEGATE_OneParam(FNotifyTimerEnd, float)

class CoolTimerContents
{
public:
	float CurrentTime;
	float EndTime;

	FTimerHandle* TimerHandle;
	FDoTimerTick DoTimerTick;
	FNotifyTimerEnd NotifyTimerEnd;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUOSHOOTING_API UCoolTimerManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCoolTimerManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	//변수
private:
	TMap<FTimerHandle*, TSharedPtr<CoolTimerContents>> CoolTimerContentsMap;
	//함수
private:
	FNotifyTimerEnd RemoveTimer(FTimerHandle* timerHandle);
public:
	template <class UserClass>
	void RegisterCoolTimerAll(UserClass* functionOwner, FTimerHandle& timerHandle, float startTime, float endTime, float inRate, void (UserClass::*doTimerTick)(float), void (UserClass::*notifyTimerEnd)(float));
	template <class UserClass>
	void RegisterCoolTimerDo(UserClass* functionOwner, FTimerHandle& timerHandle, float startTime, float endTime, float inRate, void (UserClass::*doTimerTick)(float));
	template <class UserClass>
	void RegisterCoolTimerEnd(UserClass* functionOwner, FTimerHandle& timerHandle, float startTime, float endTime, float inRate, void (UserClass::*notifyTimerEnd)(float));
};
