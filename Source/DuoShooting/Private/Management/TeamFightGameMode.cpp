// Fill out your copyright notice in the Description page of Project Settings.


#include "Management/TeamFightGameMode.h"


#include "GameFramework/PlayerStart.h"

#include "Kismet/GameplayStatics.h"
#include "Management/EnumContainer.h"
#include "Management/NetworkGameInstance.h"
#include "Management/TeamFightGameState.h"
#include "Management/TeamFightPlayerState.h"
#include "Management/TeamPlayerStart.h"
#include "Player/HeroBase.h"
#include "Player/PickPhasePawn.h"
#include "Player/SombraHero.h"
#include "Player/TracerHero.h"
#include "Algo/Sort.h"

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
	
	//default gamestate를 TeamFightGameState로 변경
	ConstructorHelpers::FClassFinder<ATeamFightGameState> gameState(TEXT("/Script/Engine.Blueprint'/Game/DuoShooting/Blueprints/GameStates/BP_TeamFightGameState.BP_TeamFightGameState_C'"));
	if (gameState.Succeeded())
	{
		GameStateClass = gameState.Class; 
	}
}

void ATeamFightGameMode::BeginPlay()
{
	Super::BeginPlay();

	//내가 서버라면, 게임 인스턴스로 부터 최소 시작 인원수를 갱신한다.
	if (GetLocalRole() == ROLE_Authority)
	{
		StartPlayerCount = GetGameInstance<UNetworkGameInstance>()->GetStartPlayerCount();
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
	
	//DrawDebugString(GetWorld(), FVector::Zero(), beforePawn->GetName(), beforePawn, FColor::Red);
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

	//위치 찾아 팀에 따라 배분
	FVector location = FVector::ZeroVector;
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeamPlayerStart::StaticClass(), PlayerStarts);
	for (AActor* each : PlayerStarts)
	{
		if (each->ActorHasTag("A"))
		{
			if (playerController->GetPlayerState<ATeamFightPlayerState>()->GetPlayerTeam() == ETeamInfo::A)
			{
				location = each->GetActorLocation();
			}
		}
		else if (each->ActorHasTag("B"))
		{
			if (playerController->GetPlayerState<ATeamFightPlayerState>()->GetPlayerTeam() == ETeamInfo::B)
			{
				location = each->GetActorLocation();
			}
		}
	}
	
	//지연 스폰을 마무리 한다.
	newHero->FinishSpawning(FTransform(location));
}

void ATeamFightGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	// 새로 참여한 플레이어의 팀 설정해주기
	if (ATeamFightPlayerState* teamFightPlayerState = Cast<ATeamFightPlayerState>(NewPlayer->PlayerState))
	{
		// A팀 수가 더 적다면 B팀으로 설정
		if (Players_TeamA.Num() < Players_TeamB.Num())
			SetPlayerTeam(NewPlayer, ETeamInfo::A);
		// B팀 수가 더 적다면 A팀으로 설정
		else if (Players_TeamB.Num() < Players_TeamA.Num())
			SetPlayerTeam(NewPlayer, ETeamInfo::B);
		// 인원수가 같다면 랜덤부여(임시)
		else
			SetPlayerTeam(NewPlayer, FMath::RandBool() ? ETeamInfo::A : ETeamInfo::B);
	}

	//// n:n매치가 결성되었다면?
	//if (Players_TeamA.Num() >= MinimumPlayerPerTeam && Players_TeamB.Num() >= MinimumPlayerPerTeam) {}

	CurrentPlayerCount++;

	//만약 시작 조건을 만족했다면 
	if (CurrentPlayerCount == StartPlayerCount)
	{
		//게임 시작 준비를 해라
		GetGameState<ATeamFightGameState>()->SetGameStartTimer();
	}
	//만약 넘는다면, 그냥 넘어간다.
	else if (CurrentPlayerCount > StartPlayerCount)
	{
		
	}
}

void ATeamFightGameMode::SetPlayerTeam(APlayerController* playerController, ETeamInfo playerTeam)
{
	// 플레이어 스테이트의 팀값 설정해주기
	if (ATeamFightPlayerState* teamFightPlayerState = Cast<ATeamFightPlayerState>(playerController->PlayerState))
	{
		teamFightPlayerState->SetPlayerTeam(playerTeam);
	}

	// 팀 리스트에 플레이어 추가
	if (playerTeam == ETeamInfo::A)
	{
		Players_TeamB.Remove(playerController);
		Players_TeamA.Add(playerController);
	}
	else if (playerTeam == ETeamInfo::B)
	{
		Players_TeamA.Remove(playerController);
		Players_TeamB.Add(playerController);
	}
}

void ATeamFightGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APlayerController* ExitingPlayer = Cast<APlayerController>(Exiting);
	if (!ExitingPlayer) return;

	// 팀에서 제거
	Players_TeamA.Remove(ExitingPlayer);
	Players_TeamB.Remove(ExitingPlayer);
}

void ATeamFightGameMode::RespawnAllPlayers()
{
	//서버에 등록된 모든 플레이어를 리스폰 한다.
	for (auto each : PlayerSpawnHeroMaps)
	{
		RespawnPlayer(each.Key);
	}
}

EHeroInfo ATeamFightGameMode::GetPlayerHero(APlayerController* playerController)
{
	return PlayerSpawnHeroMaps[playerController];
}

void ATeamFightGameMode::EndGame(ETeamInfo winnerTeam)
{
	// 우승한 팀 정보를 게임 인스턴스로 넘김
	if (auto gameInstance = Cast<UNetworkGameInstance>(GetGameInstance()))
	{
		// 승리한 팀 리스트 가져오기
		TArray<APlayerController*> Winners = winnerTeam == ETeamInfo::A ? Players_TeamA : Players_TeamB;

		// 승리한 팀원의 필수 정보(캐릭터, 점수, 유저이름) 모으기
		TArray<FFinalPlayStats> Stats_Winners;
		for (APlayerController* playerController : Winners)
		{
			FFinalPlayStats stats;
			stats.HeroInfo = PlayerSpawnHeroMaps[playerController];
			if (auto* playerState = playerController->GetPlayerState<ATeamFightPlayerState>())
			{
				stats.Score = playerState->GetMyScore();
				stats.UserName = playerState->GetUserName();
			}
			Stats_Winners.Add(stats);
		}

		// 점수 내림차순 정렬
		Algo::Sort(Stats_Winners, [](const FFinalPlayStats& A, const FFinalPlayStats& B)
		{
			return A.Score > B.Score;
		});
		
		// 정렬된 리스트를 게임인스턴스로 넘기기
		gameInstance->RememberTeamStats_Winner(Stats_Winners);
	}

	// 우승 세레모니로 넘어가기
	GetWorld()->ServerTravel("/Game/DuoShooting/Maps/Integration/EndStatisticsLevel?listen?port=7777");
}
