// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumContainer.h"
#include "GameFramework/GameState.h"
#include "TeamFightGameState.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API ATeamFightGameState : public AGameState
{
	GENERATED_BODY()

	//==기본 상속==//
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//==변수==//
protected:
	UPROPERTY(ReplicatedUsing=OnRep_Score_TeamA)
	int32 Score_TeamA = 0;
	UPROPERTY(ReplicatedUsing=OnRep_Score_TeamB)
	int32 Score_TeamB = 0;
	//==함수==//
public:
	UFUNCTION()
	void OnRep_Score_TeamA();
	UFUNCTION()
	void OnRep_Score_TeamB();
	
	void AddOneScore_TeamA();
	void AddOneScore_TeamB();

	int32 GetScore_TeamA() const { return Score_TeamA; } 
	int32 GetScore_TeamB() const { return Score_TeamB; } 

	//==변수==//
private:
	
	//시작 대기 시간
	float WaitingTime;
	//현재 남은 시작 대기 시간
	UPROPERTY(Replicated)
	float CurrentRemainWaitingTime;
	//시작 대기 타이머 핸들
	FTimerHandle WaitingTimerHandle;
	//시작 대기 여부
	UPROPERTY(Replicated)
	bool bWaiting;
	//게임 시작 여부
	UPROPERTY(Replicated)
	bool bStart;
	
	//==함수==//
private:
	// 인게임 시작!
	void StartGame();

public:
	// 게임 스타트 시간 재기
	void SetGameStartTimer();

	//남은 시간 가져오기
	float GetCurrnetRemainWaitingTime();
	//대기 진입 여부 확인하기
	bool GetIsWaiting();
	//게임 시작 여부 확인하기
	bool GetIsStart();
};
