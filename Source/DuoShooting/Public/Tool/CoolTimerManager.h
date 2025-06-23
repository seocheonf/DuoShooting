// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "Engine/TimerHandle.h"

/**
 * 
 */

class TimerContents
{
public:
	TimerContents(UObject* owner, UWorld* world, FTimerHandle timerHandle, float startTime, float endTime, float inRate,\
		TFunction<void(float, float)>&& tickCallback, TFunction<void(float)>&& endCallback);
	~TimerContents();
	
	UObject* Owner;
	UWorld* World;

	//복제하여 값을 가지고 있게 한다.
	FTimerHandle TimerHandle;

	float InRate;
	float CurrentTime;
	float EndTime;

	// 매개변수 (float, float)은 각각 tick deltaTime과 현재 타이머 진행 시각을 의미합니다.
	TFunction<void(float, float)> TickCallback;
	// 매개변수 (float)은 타이머 계산상 오버된 시각입니다.
	TFunction<void(float)> EndCallback;

	bool bEnd = false;
};

//====

class DUOSHOOTING_API CoolTimerManager
{
public:
	CoolTimerManager();
	~CoolTimerManager();
	
private:
	static TMap<FTimerHandle, TimerContents*> TimerMap;
	//지울 대상을 탐색하여 저장 후, queue를 빼가며 차근차근 대상을 Map에서 지워나가기 위한 함수.
	static TQueue<FTimerHandle> DeleteTimerQueue;
	
public:
	/// 
	/// @param World 
	/// @param InOutHandle 
	/// @param TickCallBack 매개변수 (float, float)은 각각 tick deltaTime과 현재 타이머 진행 시각을 의미합니다.
	/// @param EndCallBack 매개변수 (float)은 타이머 계산상 오버된 시간입니다.
	/// @param InRate 틱 함수가 호출되는 간격입니다.
	/// @param startTime 틱 함수가 이 시간을 기점으로 계산을 시작하여 endTime까지 다가갑니다. (최초 currentTime이 startTime이 되며, startTime이 endTime이 될 때 까지 다가갑니다)
	/// @param endTime 틱 함수가 실행된 이후 종료될 시간입니다.
	static void RegisterCoolTimerAll(UObject* Owner, UWorld* World,\
									 FTimerHandle& InOutHandle,TFunction<void(float, float)>&& TickCallBack, TFunction<void(float)>&& EndCallBack, float InRate,\
									 float startTime, float endTime);

	//외부에서 사용하는 Clear함수
	static void ClearCoolTimer(FTimerHandle& TimerHandle);

	//전체 Timer를 확인하여 종료된 대상에 대한 정보를 제거하는 함수
	static void CheckTimerContents();
	//주기적으로 전체 Timer를 확인하여 종료된 대상에 대한 정보를 제거하는 함수
	static bool TickCheckTimerContents(float deltaTime);
	
	
private:
	//내부에서 사용하는 Clear함수
	static void InClearCoolTimer(FTimerHandle& TimerHandle);
	//대상 TimerHandle에 대하여, Map에서 제거하고, 내부 Contents까지 제거하는 함수.
	static void DeleteCoolTimer(FTimerHandle& TimerHandle);
	//TimerContents객체의 메모리 점유를 해제하는 함수
	static void DeleteTimerContents(TimerContents* TimerContents);
	
};
