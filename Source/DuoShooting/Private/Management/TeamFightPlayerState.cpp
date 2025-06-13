// Fill out your copyright notice in the Description page of Project Settings.


#include "Management/TeamFightPlayerState.h"
#include "Net/UnrealNetwork.h"

void ATeamFightPlayerState::SetPlayerTeam(ETeamInfo newTeam)
{
	PlayerTeam = newTeam;
}

void ATeamFightPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ATeamFightPlayerState, PlayerTeam);
}
