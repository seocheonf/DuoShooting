// Fill out your copyright notice in the Description page of Project Settings.


#include "Management/TeamFightGameState.h"

#include "Management/TeamFightGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Player/HeroBase.h"

void ATeamFightGameState::BeginPlay()
{
	Super::BeginPlay();

	WaitingTime = 30.f;
	CurrentRemainWaitingTime = WaitingTime;

}

void ATeamFightGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamFightGameState, Score_TeamA);
	DOREPLIFETIME(ATeamFightGameState, Score_TeamB);

	DOREPLIFETIME(ATeamFightGameState, CurrentRemainWaitingTime);
	DOREPLIFETIME(ATeamFightGameState, bWaiting);
	DOREPLIFETIME(ATeamFightGameState, bStart);
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

void ATeamFightGameState::StartGame()
{
	//게임 모드로 부터 모든 플레이어를 리스폰 하라는 요청을 진행한다.
	GetWorld()->GetAuthGameMode<ATeamFightGameMode>()->RespawnAllPlayers();
}

void ATeamFightGameState::SetGameStartTimer()
{
	//시간 측정이 완료되면 게임 시작을 진행한다.
	auto checkTime = [&]()
	{
		CurrentRemainWaitingTime -= GetWorld()->GetDeltaSeconds();
		if (CurrentRemainWaitingTime <= 0)
		{
			CurrentRemainWaitingTime = 0;
			//대기 여부도 다시 꺼준다.
			bWaiting = false;
			//시작 여부를 켜준다.,
			bStart = true;
			StartGame();
			GetWorldTimerManager().ClearTimer(WaitingTimerHandle);
		}
	};

	FTimerManagerTimerParameters params;
	params.bLoop = true;
	params.bMaxOncePerFrame = true;
	
	GetWorldTimerManager().SetTimer(WaitingTimerHandle, checkTime, 0.003f, params);

	bWaiting = true;
}

float ATeamFightGameState::GetCurrnetRemainWaitingTime()
{
	return CurrentRemainWaitingTime;
}

bool ATeamFightGameState::GetIsWaiting()
{
	return bWaiting;
}

bool ATeamFightGameState::GetIsStart()
{
	return bStart;
}
