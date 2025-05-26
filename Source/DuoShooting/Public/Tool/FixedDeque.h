// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//트레이서의 시간역행을 위한 커스텀 컨테이너 
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
	void Push_Back(const T& element);
	T Pop_Back(bool& OutEmpty);
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
void FixedDeque<T>::Push_Back(const T& element)
{
	Elements[CurrentHead] = element;
	CurrentHead = (CurrentHead + 1) % MaxSize;
	CurrentSize = FMath::Min(CurrentSize + 1, MaxSize);
}

template <typename T>
T FixedDeque<T>::Pop_Back(bool& OutEmpty)
{
	CurrentHead = (CurrentHead - 1) % MaxSize;
	CurrentSize = FMath::Max(CurrentSize - 1 + MaxSize, 0);
	OutEmpty = CurrentSize == 0;
	return Elements[CurrentHead];
}


