// Fill out your copyright notice in the Description page of Project Settings.


#include "Management/TeamFightGameState.h"

#include "Management/TeamFightGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Player/HeroBase.h"

void ATeamFightGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ATeamFightGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamFightGameState, Score_TeamA);
	DOREPLIFETIME(ATeamFightGameState, Score_TeamB);
}

void ATeamFightGameState::OnRep_Score_TeamA()
{
	if (AHeroBase* localHero = Cast<AHeroBase>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		localHero->UpdateTeamScoreUI();
	}
}

void ATeamFightGameState::OnRep_Score_TeamB()
{
	if (AHeroBase* localHero = Cast<AHeroBase>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		localHero->UpdateTeamScoreUI();
	}
}

void ATeamFightGameState::AddOneScore_TeamA()
{
	Score_TeamA++;
	UE_LOG(LogTemp, Warning, TEXT("Team A Score: %d"), Score_TeamA);

	OnRep_Score_TeamA();
}

void ATeamFightGameState::AddOneScore_TeamB()
{
	Score_TeamB++;
	UE_LOG(LogTemp, Warning, TEXT("Team B Score: %d"), Score_TeamB);

	OnRep_Score_TeamB();
}
