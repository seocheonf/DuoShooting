// Fill out your copyright notice in the Description page of Project Settings.


#include "Management/TeamFightGameMode.h"

#include "Management/EnumContainer.h"
#include "Management/TeamFightGameState.h"
#include "Management/TeamFightPlayerState.h"
#include "Player/HeroBase.h"
#include "Player/PickPhasePawn.h"
#include "Player/SombraHero.h"
#include "Player/TracerHero.h"

ATeamFightGameMode::ATeamFightGameMode()
{
	//트레이서 UClass 정보를 파일로 받아와서 저장 (TSubclassOf 활용)
	ConstructorHelpers::FClassFinder<ATracerHero> tracer(TEXT("/Script/Engine.Blueprint'/Game/DuoShooting/Blueprints/Characters/BP_Tracer.BP_Tracer_C'"));
	if (tracer.Succeeded())
	{
		HeroSourceMap.Add(EHeroInfo::Tracer, tracer.Class);
	}
	//솜브라 UClass 정보를 파일로 받아와서 저장 (TSubclassOf 활용)
	ConstructorHelpers::FClassFinder<ASombraHero> sombra(TEXT("/Script/Engine.Blueprint'/Game/DuoShooting/Blueprints/Characters/Sombra/BP_Sombra.BP_Sombra_C'"));
	if (sombra.Succeeded())
	{
		HeroSourceMap.Add(EHeroInfo::Sombra, sombra.Class);
	}

	//default pawn을 PickPhasePawn으로 변경
	ConstructorHelpers::FClassFinder<APickPhasePawn> pawn(TEXT("/Script/Engine.Blueprint'/Game/DuoShooting/Blueprints/Characters/BP_PickPhasePawn.BP_PickPhasePawn_C'"));
	if (pawn.Succeeded())
	{
		DefaultPawnClass = pawn.Class; 
	}
	
	//default gamestate를 TeamFightGameState로 변경
	ConstructorHelpers::FClassFinder<ATeamFightGameState> gameState(TEXT("/Script/Engine.Blueprint'/Game/DuoShooting/Blueprints/GameStates/BP_TeamFightGameState.BP_TeamFightGameState_C'"));
	if (gameState.Succeeded())
	{
		GameStateClass = gameState.Class; 
	}
	
}

void ATeamFightGameMode::SetPlayerHero(APlayerController* playerController, EHeroInfo playerHero)
{
	//이미 값이 존재하면 값을 덮어 씌움. 없다면 추가.
	//플레이어가 무슨 캐릭터로 태어날지 정보를 기록해 둔다.
	PlayerSpawnHeroMaps.Add(playerController, playerHero);
}

void ATeamFightGameMode::RespawnPlayer(APlayerController* playerController)
{
	//플레이어 폰을 가져와 둔다.
	APawn* beforePawn = playerController->GetPawn();
	DrawDebugString(GetWorld(), FVector::Zero(), beforePawn->GetName(), beforePawn, FColor::Red);
	//플레이어 포제스를 푼다.
	playerController->UnPossess();
	//플레이어 기존 폰을 파괴한다.
	beforePawn->Destroy();
	//플레이어 컨트롤러에 대응되었던 (저장해 두었던) 캐릭터 정보를 불러옴
	EHeroInfo playerHero = PlayerSpawnHeroMaps[playerController];
	//캐릭터 정보를 바탕으로 UClass 정보를 가져옴.
	TSubclassOf<AHeroBase> heroSource = HeroSourceMap[playerHero];
	//UClass 정보를 바탕으로 실제 대상을 스폰함
	//지연 스폰하여 BeginPlay등의 준비를 하기전에, 미리 Possess 작업을 선수쳐둔다.
	//AHeroBase* newHero = GetWorld()->SpawnActor<AHeroBase>(heroSource);
	AHeroBase* newHero = GetWorld()->SpawnActorDeferred<AHeroBase>(heroSource, FTransform(), nullptr, nullptr);
	//플레이어 컨트롤러에 새로운 캐릭터를 Possess 시킴.
	playerController->Possess(newHero);
	//지연 스폰을 마무리 한다.
	newHero->FinishSpawning(FTransform());
}

void ATeamFightGameMode::SetPlayerTeam(APlayerController* playerController, ETeamInfo playerTeam)
{
	// 플에이어가 무슨 팀으로 태어날지 정모를 기록해 둔다
	PlayerSpawnTeamMaps.Add(playerController, playerTeam);

	// 플레이어 스테이트
	if (ATeamFightPlayerState* teamFightPlayerState = Cast<ATeamFightPlayerState>(playerController->PlayerState))
	{
		teamFightPlayerState->SetPlayerTeam(playerTeam);
	}
}
