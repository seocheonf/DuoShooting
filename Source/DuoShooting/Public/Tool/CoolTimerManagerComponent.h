// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CoolTimerManagerComponent.generated.h"

/**
 * 쿨 계산 매 틱마다 할 일. 매개변수는 DeltaSeconds, 현재 진행 시간
 */
DECLARE_DELEGATE_TwoParams(FDoTimerTick, float, float)
/**
 * 쿨타임 종료 시 할 일. 매개변수는 DeltaSeconds 계산으로 인해 남게 되는 시간.
 */
DECLARE_DELEGATE_OneParam(FNotifyTimerEnd, float)

class CoolTimerContents
{
public:
	//쿨타임 게산 현재 시간
	float CurrentTime;
	//쿨타임 계산 종료 시간
	float EndTime;

	//쿨타임 계산 제어 핸들
	FTimerHandle TimerHandle;
	//쿨타임 지속 시간동안 할 일
	FDoTimerTick DoTimerTick;
	//쿨타임 종료시 할 일
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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	//변수
private:
	//쿨타임 관리 대상이 되는 TimerHandle에 대해 쿨타임 계산에 필요한 정보 저장을 위한 자료구조.
	TMap<FTimerHandle, TSharedPtr<CoolTimerContents>> CoolTimerContentsMap;
	//함수
private:
public:
	//쿨타임 관리 대상으로 부터 제거. notify end는 무시됨
	FNotifyTimerEnd RemoveTimer(FTimerHandle timerHandle);

	/**
	 * 쿨타임 도중 할 일과, 쿨타임 종료 시 할 일을 모두 설정한다.
	 * @tparam UserClass 함수의 주인 객체의 클래스
	 * @param functionOwner 함수의 주인 객체
	 * @param timerHandle 개별 쿨타임 제어용 핸들
	 * @param startTime 쿨타임 계산 시작 시간 (seconds) / (endTime-startTime) 시간동안 쿨타임 계산 지속
	 * @param endTime 쿨타임 종료 시간 (seconds) / (endTime-startTime) 시간동안 쿨타임 계산 지속
	 * @param inRate 타이머 호출 간격 (seconds)
	 * @param doTimerTick 타이머 매 틱 할 일
	 * @param notifyTimerEnd 타이머 종료시 할 일
	 */
	template <class UserClass>
	void RegisterCoolTimerAll(UserClass* functionOwner, FTimerHandle& timerHandle, float startTime, float endTime, float inRate, void (UserClass::*doTimerTick)(float, float), void (UserClass::*notifyTimerEnd)(float))
	{
		UE_LOG(LogTemp, Error, TEXT("register start"));

		if (CoolTimerContentsMap.Contains(timerHandle))
		{
			UE_LOG(LogTemp, Error, TEXT("register inner"));
			CoolTimerContentsMap[timerHandle] = nullptr;
			CoolTimerContentsMap.Remove(timerHandle);
			GetWorld()->GetTimerManager().ClearTimer(timerHandle);
		}
	
		TSharedPtr<CoolTimerContents> coolTimerContents = MakeShared<CoolTimerContents>();
	
		coolTimerContents->CurrentTime = startTime;
		coolTimerContents->EndTime = endTime;

		if (nullptr != doTimerTick)
			coolTimerContents->DoTimerTick.BindUObject(functionOwner, doTimerTick);
		if (nullptr != notifyTimerEnd)
			coolTimerContents->NotifyTimerEnd.BindUObject(functionOwner, notifyTimerEnd);

		FTimerManagerTimerParameters timerParameters(true, true);
	
		GetWorld()->GetTimerManager().SetTimer(timerHandle,
			[this, coolTimerContents]()
			{
				coolTimerContents->CurrentTime += GetWorld()->DeltaTimeSeconds;
				if (coolTimerContents->DoTimerTick.IsBound())
				{
					coolTimerContents->DoTimerTick.Execute(GetWorld()->DeltaTimeSeconds, coolTimerContents->CurrentTime);
				}
				if (coolTimerContents->CurrentTime >= coolTimerContents->EndTime)
				{
					float excessDeltaTime = coolTimerContents->CurrentTime - coolTimerContents->EndTime;
					FNotifyTimerEnd notifyEnd = RemoveTimer(coolTimerContents->TimerHandle);
					if (notifyEnd.IsBound())
					{
						notifyEnd.Execute(excessDeltaTime);
					}
				}
			
			}, inRate, timerParameters);
	
		coolTimerContents->TimerHandle = timerHandle;
		CoolTimerContentsMap.Add(timerHandle, coolTimerContents);
	};

	/**
	 * 쿨타임을 등록하는 기능이다.
	 * 쿨타임 도중 할 일을 설정한다.
	 * @tparam UserClass 함수의 주인 객체의 클래스
	 * @param functionOwner 함수의 주인 객체
	 * @param timerHandle 개별 쿨타임 제어용 핸들
	 * @param startTime 쿨타임 계산 시작 시간 (seconds) / (endTime-startTime) 시간동안 쿨타임 계산 지속
	 * @param endTime 쿨타임 종료 시간 (seconds) / (endTime-startTime) 시간동안 쿨타임 계산 지속
	 * @param inRate 타이머 호출 간격 (seconds)
	 * @param doTimerTick 타이머 매 틱 할 일
	 */
	template <class UserClass>
	void RegisterCoolTimerDo(UserClass* functionOwner, FTimerHandle& timerHandle, float startTime, float endTime, float inRate, void (UserClass::*doTimerTick)(float, float))
	{
		RegisterTimerAll(functionOwner, timerHandle, startTime, endTime, inRate, doTimerTick, static_cast<void(UserClass::*)(float)>(nullptr));
	};

	/**
	 * 쿨타임을 등록하는 기능이다.
	 * 쿨타임 종료 시 할 일을 설정한다.
	 * @tparam UserClass 함수의 주인 객체의 클래스
	 * @param functionOwner 함수의 주인 객체
	 * @param timerHandle 개별 쿨타임 제어용 핸들
	 * @param startTime 쿨타임 계산 시작 시간 (seconds) / (endTime-startTime) 시간동안 쿨타임 계산 지속
	 * @param endTime 쿨타임 종료 시간 (seconds) / (endTime-startTime) 시간동안 쿨타임 계산 지속
	 * @param inRate 타이머 호출 간격 (seconds)
	 * @param notifyTimerEnd 타이머 종료시 할 일
	 */
	template <class UserClass>
	void RegisterCoolTimerEnd(UserClass* functionOwner, FTimerHandle& timerHandle, float startTime, float endTime, float inRate, void (UserClass::*notifyTimerEnd)(float))
	{
		RegisterTimerAll(functionOwner, timerHandle, startTime, endTime, inRate, static_cast<void(UserClass::*)(float)>(nullptr), notifyTimerEnd);
	};

	/**
     * 쿨타임을 등록하는 기능이다.
     * 쿨타임 도중 할 일을 설정한다.
     * 델리게이트 매개변수로, 람다식을 넘길 수도 있다.
     * @param timerHandle 개별 쿨타임 제어용 핸들
     * @param startTime 쿨타임 계산 시작 시간 (seconds) / (endTime-startTime) 시간동안 쿨타임 계산 지속
     * @param endTime 쿨타임 종료 시간 (seconds) / (endTime-startTime) 시간동안 쿨타임 계산 지속
     * @param inRate 타이머 호출 간격 (seconds)
     * @param doTimerTick 타이머 매 틱 할 일 (델리게이트)
     * @param notifyTimerEnd 타이머 종료시 할 일 (델리게이트)
     */
	void RegisterCoolTimerAll(FTimerHandle& timerHandle, float startTime, float endTime, float inRate, FDoTimerTick& doTimerTick, FNotifyTimerEnd& notifyTimerEnd);

};
