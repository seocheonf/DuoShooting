// Fill out your copyright notice in the Description page of Project Settings.


#include "Management/EndStatisticsGameModeBase.h"

#include "Management/NetworkGameInstance.h"
#include "Management/TeamFightGameMode.h"
#include "Player/EndStatisticsHelperActor.h"

void AEndStatisticsGameModeBase::CreateActorHelpers()
{
	if (HelperActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("HelperActor is null"));
		return;
	}
	
	FVector spawnLocation = FVector::ZeroVector;

	if (auto* gameInstance = Cast<UNetworkGameInstance>(GetGameInstance()))
	{
		// 승리한 팀의 플레이어 리스트 가져오기
		TArray<FFinalPlayStats> winners = gameInstance->GetWinnerTeamStats();

		if (winners.Num() > 0)
		{
			// 승리한 팀을 순회하면서
			for (FFinalPlayStats winner : winners)
			{
				// 더미 액터 스폰
				auto spawnedHelperActor = GetWorld()->SpawnActor<AEndStatisticsHelperActor>(
					HelperActor, spawnLocation, FRotator::ZeroRotator);

				// 액터에 플레이어 정보 설정
				spawnedHelperActor->Server_SetPlayStats(winner);

				// 다음 스폰 위치
				spawnLocation.X = spawnLocation.X - 80.0f;
				spawnLocation.Y = spawnLocation.Y + 90.0f;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Winners empty"));

			// 더미 액터 스폰
			auto spawnedHelperActor = GetWorld()->SpawnActor<AEndStatisticsHelperActor>(
				HelperActor, spawnLocation, FRotator::ZeroRotator);

			// 액터에 플레이어 정보 설정
			FFinalPlayStats dummy;
			dummy.HeroInfo = EHeroInfo::Tracer;
			dummy.UserName = TEXT("더미 액터");
			dummy.Score = -77;
			spawnedHelperActor->Server_SetPlayStats(dummy);
		}
	}
}

AEndStatisticsGameModeBase::AEndStatisticsGameModeBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	{
		ConstructorHelpers::FClassFinder<AEndStatisticsHelperActor> tempActor(TEXT(
			"/Script/Engine.Blueprint'/Game/DuoShooting/Blueprints/EndGameStatistics/BP_EndStatisticsHelperActor.BP_EndStatisticsHelperActor_C'"));
		if (tempActor.Succeeded()) HelperActor = tempActor.Class;
	}
}

void AEndStatisticsGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	CreateActorHelpers();
}