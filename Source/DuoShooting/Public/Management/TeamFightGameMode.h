// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumContainer.h"
#include "GameFramework/GameMode.h"
#include "TeamFightGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API ATeamFightGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATeamFightGameMode();
	virtual void BeginPlay() override;
	
	//==변수==//
private:
	//플레이어와 대응되는 영웅 정보
	TMap<EHeroInfo, TSubclassOf<class AHeroBase>> HeroSourceMap;
	TMap<APlayerController*, EHeroInfo> PlayerSpawnHeroMaps;
	
	// 팀 관련

	// 최소 시작 인원
	int32 StartPlayerCount = 1;
	// 현재 입장 인원
	int32 CurrentPlayerCount = 0;

	// 이기기 위해 모아야할 점수
	int32 TargetWinScore = 2;

	// 팀 A의 멤버들
	UPROPERTY()
	TArray<APlayerController*> Players_TeamA;

	// 팀 B의 멤버들
	UPROPERTY()
	TArray<APlayerController*> Players_TeamB;

	//==함수==//
	
	
public:
	//대상이 되는 플레이어의 영웅 정보를 갱신하는 함수
	void SetPlayerHero(APlayerController* playerController, EHeroInfo playerHero);

	//대상이 되는 플레이어의 영웅 정보에 따라 리스폰 해주는 함수
	void RespawnPlayer(APlayerController* playerController);
	
	// PJW: 대상이 되는 플레이어의 팀 정보를 갱신하는 함수
	void SetPlayerTeam(APlayerController* playerController, ETeamInfo playerTeam);

	// 해당 플레이어가 팀에 참가했을 때 설정할 것
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 해당 플레이어가 로그아웃할 때 설정할 것
	virtual void Logout(AController* Exiting) override;

	//KHM
	//모든 플레이어를 리스폰 하는 함수
	void RespawnAllPlayers();

	//PJW
	int32 GetTargetWinScore() const { return TargetWinScore; }

	// 컨트롤러의 캐릭터 매핑 정보를 가져온다
	EHeroInfo GetPlayerHero(APlayerController* playerController);

	// 게임을 끝내고 랭킹씬으로 나감
	void EndGame(ETeamInfo winnerTeam);
};

