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
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//==변수==//
private:
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
};
