// Fill out your copyright notice in the Description page of Project Settings.


#include "Tool/CoolTimerManagerComponent.h"


// Sets default values for this component's properties
UCoolTimerManagerComponent::UCoolTimerManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCoolTimerManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCoolTimerManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FNotifyTimerEnd UCoolTimerManagerComponent::RemoveTimer(FTimerHandle* timerHandle)
{
	GetWorld()->GetTimerManager().ClearTimer(*timerHandle);
	FNotifyTimerEnd notifyEnd;
	if (CoolTimerContentsMap.Contains(timerHandle))
	{
		notifyEnd = CoolTimerContentsMap[timerHandle]->NotifyTimerEnd;
		
		CoolTimerContentsMap[timerHandle] = nullptr;
		CoolTimerContentsMap.Remove(timerHandle);
	}
	return notifyEnd;
}

template <class UserClass>
void UCoolTimerManagerComponent::RegisterCoolTimerAll(UserClass* functionOwner, FTimerHandle& timerHandle, float startTime,
	float endTime, float inRate, void(UserClass::* doTick)(float), void(UserClass::* notifyEnd)(float))
{
	UE_LOG(LogTemp, Error, TEXT("register start"));

	if (CoolTimerContentsMap.Contains(&timerHandle))
	{
		UE_LOG(LogTemp, Error, TEXT("register inner"));
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);
		CoolTimerContentsMap[&timerHandle] = nullptr;
		CoolTimerContentsMap.Remove(&timerHandle);
	}
	
	TSharedPtr<CoolTimerContents> coolTimerContents = MakeShared<CoolTimerContents>();
	CoolTimerContentsMap.Add(&timerHandle, coolTimerContents);
	
	coolTimerContents->CurrentTime = startTime;
	coolTimerContents->EndTime = endTime;
	coolTimerContents->TimerHandle = &timerHandle;

	if (nullptr != doTick)
		coolTimerContents->DoTimerTick.BindUObject(functionOwner, doTick);
	if (nullptr != notifyEnd)
		coolTimerContents->NotifyTimerEnd.BindUObject(functionOwner, notifyEnd);

	FTimerManagerTimerParameters timerParameters(true, true);
	
	GetWorld()->GetTimerManager().SetTimer(timerHandle,
		[this, coolTimerContents]()
		{
			coolTimerContents->CurrentTime += GetWorld()->DeltaTimeSeconds;
			if (coolTimerContents->DoTimerTick.IsBound())
			{
				coolTimerContents->DoTimerTick.Execute(GetWorld()->DeltaTimeSeconds);
			}
			if (coolTimerContents->CurrentTime >= coolTimerContents->EndTime)
			{
				FNotifyTimerEnd notifyEnd = RemoveTimer(coolTimerContents->TimerHandle);
				if (notifyEnd.IsBound())
				{
					notifyEnd.Execute(0.1f);
				}
			}
			
		}, inRate, timerParameters);
}

template <class UserClass>
void UCoolTimerManagerComponent::RegisterCoolTimerDo(UserClass* functionOwner, FTimerHandle& timerHandle, float startTime,
	float endTime, float inRate, void(UserClass::* doTimerTick)(float))
{
	RegisterTimerAll(functionOwner, timerHandle, startTime, endTime, inRate, doTimerTick, static_cast<void(UserClass::*)(float)>(nullptr));
}

template <class UserClass>
void UCoolTimerManagerComponent::RegisterCoolTimerEnd(UserClass* functionOwner, FTimerHandle& timerHandle, float startTime,
	float endTime, float inRate, void(UserClass::* notifyTimerEnd)(float))
{
	RegisterTimerAll(functionOwner, timerHandle, startTime, endTime, inRate, static_cast<void(UserClass::*)(float)>(nullptr), notifyTimerEnd);
}

