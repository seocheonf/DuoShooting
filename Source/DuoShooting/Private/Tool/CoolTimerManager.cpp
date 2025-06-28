// Fill out your copyright notice in the Description page of Project Settings.


#include "Tool/CoolTimerManager.h"

TimerContents::TimerContents(UObject* owner, UWorld* world, FTimerHandle timerHandle, float startTime, float endTime, float inRate,\
							 TFunction<void(float, float)>&& tickCallback, TFunction<void(float)>&& endCallback)
{
	Owner = owner;
	
	World = world;

	TimerHandle = timerHandle;

	InRate = inRate;
	CurrentTime = startTime;
	EndTime = endTime;

	TickCallback = MoveTemp(tickCallback);
	EndCallback = MoveTemp(endCallback);
}

TimerContents::~TimerContents()
{
	Owner = nullptr;
	World = nullptr;
	
	TickCallback = nullptr;
	EndCallback = nullptr;
}

CoolTimerManager::CoolTimerManager()
{
}

CoolTimerManager::~CoolTimerManager()
{
}

TMap<FTimerHandle, TimerContents*> CoolTimerManager::TimerMap;
TQueue<FTimerHandle> CoolTimerManager::DeleteTimerQueue;

void CoolTimerManager::RegisterCoolTimerAll(UObject* Owner, UWorld* World, FTimerHandle& InOutHandle,\
	TFunction<void(float, float)>&& TickCallBack, TFunction<void(float)>&& EndCallBack, float InRate,\
	float startTime, float endTime)
{
	TimerContents* contents = new TimerContents(Owner, World, InOutHandle, startTime, endTime, InRate, MoveTemp(TickCallBack), MoveTemp(EndCallBack));
	
	FTimerManagerTimerParameters params;
	params.bLoop = true;
	params.bMaxOncePerFrame = true;

	auto timer = [contents]()
	{
		//주인이 사라졌다면 나가기
		if (false == IsValid(contents->Owner))
		{
			InClearCoolTimer(contents->TimerHandle);
			return;
		}
		
		//월드가 사라졌다면 나가기
		if (false == IsValid(contents->World))
		{
			InClearCoolTimer(contents->TimerHandle);
			return;
		}

		//종료가 되었다면 나가기
		if (contents->bEnd)
			return;

		float deltaTime = FMath::Max(contents->World->GetDeltaSeconds(), contents->InRate);
		contents->CurrentTime += deltaTime;
		
		contents->TickCallback(deltaTime, contents->CurrentTime);

		//쿨타임이 종료되었다면 마무리 함수 실행
		if (contents->CurrentTime >= contents->EndTime)
		{
			float exceedTime = contents->CurrentTime - contents->EndTime;
			contents->EndCallback(exceedTime);

			InClearCoolTimer(contents->TimerHandle);
		}
	};
	
	World->GetTimerManager().SetTimer(InOutHandle, timer, InRate, params);

	contents->TimerHandle = InOutHandle;
	TimerMap.Add(InOutHandle, contents);
}

void CoolTimerManager::ClearCoolTimer(FTimerHandle& TimerHandle)
{
	if (false == TimerMap.Contains(TimerHandle))
		return;

	//종료 태그만 설정.
	TimerMap[TimerHandle]->bEnd = true;

	//기존 타이머 핸들은 비워서 보내주기
	TimerHandle.Invalidate();
}

void CoolTimerManager::InClearCoolTimer(FTimerHandle& TimerHandle)
{
	if (false == TimerMap.Contains(TimerHandle))
		return;

	//종료 태그만 설정.
	TimerMap[TimerHandle]->bEnd = true;
}

void CoolTimerManager::CheckTimerContents()
{
	for (auto each : TimerMap)
	{
		//만약 타이머가 종료되어 있다면 => 로직 상 불가해야 함. 만약 조건이 성립한다면, 로직이 잘못되었을 가능성이 큼
		if (false == each.Key.IsValid())
		{}

		//주인이 먼저 날아가 버렸다면
		if (false == IsValid(each.Value->Owner))
		{
			DeleteTimerQueue.Enqueue(each.Key);
			continue;
		}

		//타이머가 등록된 월드가 먼저 날아가 버렸다면
		if (false == IsValid(each.Value->World))
		{
			DeleteTimerQueue.Enqueue(each.Key);
			continue;
		}
		
		//종료된 대상이라면, 삭제 대기열에 등록
		if (each.Value->bEnd)
		{
			DeleteTimerQueue.Enqueue(each.Key);
			continue;
		}
	}

	while (false == DeleteTimerQueue.IsEmpty())
	{
		FTimerHandle deleteTimer;
		DeleteTimerQueue.Dequeue(deleteTimer);
		DeleteCoolTimer(deleteTimer);
	}
}

bool CoolTimerManager::TickCheckTimerContents(float deltaTime)
{
	CheckTimerContents();
	return true;
}

void CoolTimerManager::DeleteCoolTimer(FTimerHandle& TimerHandle)
{
	//조건에 맞는 정보가 없다면
	if (false == TimerMap.Contains(TimerHandle))
		return;

	//만약 대상 타이머의 world가 사라져버린 후라면
	if (false == IsValid(TimerMap[TimerHandle]->World))
	{
		//곧바로 자료구조에서 지워버리기.
		DeleteTimerContents(TimerMap[TimerHandle]);
		TimerMap.Remove(TimerHandle);
		TimerHandle.Invalidate();
		return;
	}

	//일반적인 상황에서 대상의 타이머를 제거하는 함수
	{
		//해당 월드의 타이머에서 등록해둔 타이머를 해제하기
		TimerMap[TimerHandle]->World->GetTimerManager().ClearTimer(TimerMap[TimerHandle]->TimerHandle);
		DeleteTimerContents(TimerMap[TimerHandle]);
		TimerMap.Remove(TimerHandle);
		TimerHandle.Invalidate();
		return;
	}
}

void CoolTimerManager::DeleteTimerContents(TimerContents* TimerContents)
{
	delete(TimerContents);
}
