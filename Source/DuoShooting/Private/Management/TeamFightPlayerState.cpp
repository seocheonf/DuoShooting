// Fill out your copyright notice in the Description page of Project Settings.


#include "Management/TeamFightPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/HeroBase.h"

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
}

void ATeamFightPlayerState::OnRep_PlayerTeam()
{
	
}
