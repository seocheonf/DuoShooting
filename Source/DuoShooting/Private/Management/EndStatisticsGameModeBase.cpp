// Fill out your copyright notice in the Description page of Project Settings.


#include "Management/EndStatisticsGameModeBase.h"

#include "EngineUtils.h"
#include "Camera/CameraActor.h"

void AEndStatisticsGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// 공용 카메라를 월드에서 찾아오기
	ACameraActor* endGameCamera = nullptr;
	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	{
		if (It->GetName().Contains("EndGameCamera"))
		{
			endGameCamera = *It;
			break;
		}
	}

	// 현재 카메라로 설정
	if (endGameCamera)
	{
		if (ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController())
		{
			if (APlayerController* playerController = localPlayer->GetPlayerController(GetWorld()))
			{
				playerController->SetViewTarget(endGameCamera);
				UE_LOG(LogTemp, Warning, TEXT("View Target Set to EndGameCamera"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("cannot Get Player Controller"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("cannot Get Local Player"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("endGameCamera null"));
	}
			
}
