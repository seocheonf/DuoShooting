// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// 트레이서의 시간역행을 위한 커스텀 컨테이너
// 기록은 큐같이, 가져올때는 스택같이
template <typename T>
class DUOSHOOTING_API FixedDeque
{
	TArray<T> Elements;
	int32 MaxSize;
	int32 CurrentHead;
	int32 CurrentSize;
	
public:
	FixedDeque();
	~FixedDeque();

	void Init(int32 maxSize);
	void Clear(); 
	void Push_Back(const T& element);
	T Pop_Back(bool& Valid);
};

template <typename T>
FixedDeque<T>::FixedDeque()
{
	MaxSize = 0;
	CurrentHead = 0;
	CurrentSize = 0;
}

template <typename T>
FixedDeque<T>::~FixedDeque()
{
	Elements.Empty();
}

template <typename T>
void FixedDeque<T>::Init(int32 maxSize)
{
	if (maxSize > 0) MaxSize = maxSize;
	else { UE_LOG(LogTemp, Warning, TEXT("Invalid FixedDeque Size")); }

	Elements.SetNum(MaxSize);
}

template <typename T>
void FixedDeque<T>::Clear()
{
	CurrentSize = 0;
}

template <typename T>
void FixedDeque<T>::Push_Back(const T& element)
{
	// 현재 위치에 원소 기록
	T debugElement = element;
	Elements[CurrentHead] = element;

	// 헤드 오른쪽으로 옮기기
	CurrentHead = (CurrentHead + 1) % MaxSize;

	// 사이즈 늘리기
	if (CurrentSize < MaxSize) CurrentSize++;
}

// 컨테이너가 이미 비어있는 경우에는 Valid = false를 반환하며, 이때의 T는 쓰레기값
template <typename T>
T FixedDeque<T>::Pop_Back(bool& Valid)
{
	if (CurrentSize > 0)
	{
		// 헤드 왼쪽으로 옮기기
		CurrentHead = (CurrentHead - 1 + MaxSize) % MaxSize;

		// 사이즈 줄이기
		if (CurrentSize > 0) CurrentSize--;

		Valid = true;
	}
	else
	{
		Valid = false;
	}
	
	// 현재 위치의 원소 뱉기
	return Elements[CurrentHead];
}


