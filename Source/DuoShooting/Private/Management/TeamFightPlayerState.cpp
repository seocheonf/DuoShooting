// Fill out your copyright notice in the Description page of Project Settings.


#include "Management/TeamFightPlayerState.h"

#include "Management/TeamFightGameState.h"
#include "Net/UnrealNetwork.h"
#include "Player/HeroBase.h"
#include "UI/ShootingMainWidget.h"

void ATeamFightPlayerState::SetPlayerTeam(ETeamInfo newTeam)
{
	if (!HasAuthority()) return;
	
	PlayerTeam = newTeam;
	OnRep_PlayerTeam();
}

void ATeamFightPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ATeamFightPlayerState, PlayerTeam);
	DOREPLIFETIME(ATeamFightPlayerState, MyScore);
}

void ATeamFightPlayerState::OnRep_PlayerTeam()
{
	
}

void ATeamFightPlayerState::OnRep_MyScore()
{
	// 로컬 플레이어의 게임스테이트라면
	if (IsOwnedBy(GetWorld()->GetFirstPlayerController()))
	{
		APawn* playerPawn = GetPawn();
		if (playerPawn == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ATeamFightPlayerState::OnRep_MyScore: playerPawn null"));
			return;
		}
	
		if (AHeroBase* hero = Cast<AHeroBase>(playerPawn))
		{
			hero->UpdateMyScoreUI();
			UE_LOG(LogTemp, Warning, TEXT("ATeamFightPlayerState::OnRep_MyScore: Successfully executed"));
		}		
	}
}

// 서버에서만 불릴 것
void ATeamFightPlayerState::AddOneScore()
{
	MyScore++;
	UE_LOG(LogTemp, Warning, TEXT("MyScore: %d"), MyScore);

	OnRep_MyScore();

	// 팀스코어에도 기여하기
	if (ATeamFightGameState* teamFightGameState = GetWorld()->GetGameState<ATeamFightGameState>())
	{
		switch (PlayerTeam)
		{
		case ETeamInfo::A:
			teamFightGameState->AddOneScore_TeamA();
			break;
		case ETeamInfo::B:
			teamFightGameState->AddOneScore_TeamB();
			break;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ATeamFightPlayerState::AddOneScore: teamFightGameState is null"));
	}
}

int32 ATeamFightPlayerState::GetMyTeamScore() const
{
	int myTeamScore = -1;

	// 내편 점수 가져오기
	if (ATeamFightGameState* teamFightGameState = GetWorld()->GetGameState<ATeamFightGameState>())
	{
		switch (PlayerTeam)
		{
		case ETeamInfo::A:
			myTeamScore = teamFightGameState->GetScore_TeamA();
			break;
		case ETeamInfo::B:
			myTeamScore = teamFightGameState->GetScore_TeamB();
			break;
		}
	}

	return myTeamScore;
}

int32 ATeamFightPlayerState::GetEnemyTeamScore() const
{
	int enemyTeamScore = -1;

	// 반대편 점수 가져오기
	if (ATeamFightGameState* teamFightGameState = GetWorld()->GetGameState<ATeamFightGameState>())
	{
		switch (PlayerTeam)
		{
		case ETeamInfo::A:
			enemyTeamScore = teamFightGameState->GetScore_TeamB();
			break;
		case ETeamInfo::B:
			enemyTeamScore = teamFightGameState->GetScore_TeamA();
			break;
		}
	}

	return enemyTeamScore;
}
