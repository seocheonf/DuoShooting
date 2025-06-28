// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TeamFightPlayerState.generated.h"

enum class ETeamInfo : uint8;
/**
 * 
 */
UCLASS()
class DUOSHOOTING_API ATeamFightPlayerState : public APlayerState
{
	GENERATED_BODY()

	//사용자 이름
	UPROPERTY(ReplicatedUsing=OnRep_UserName)
	FString UserName;
	
protected:
	UPROPERTY(Replicated)
	ETeamInfo PlayerTeam;

	// 개인 스코어(킬수)
	UPROPERTY(ReplicatedUsing=OnRep_MyScore)
	int32 MyScore = 0;

public:
	void SetPlayerTeam(ETeamInfo newTeam);
	ETeamInfo GetPlayerTeam() const { return PlayerTeam; }
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_UserName();

	UFUNCTION()
	void OnRep_MyScore();

	// 킬수 1점올리기
	void AddOneScore();
	int32 GetMyScore() const { return MyScore; }

	int32 GetMyTeamScore() const;
	int32 GetEnemyTeamScore() const;

	void SetUserName(const FString& newUserName);
	const FString& GetUserName();
};
