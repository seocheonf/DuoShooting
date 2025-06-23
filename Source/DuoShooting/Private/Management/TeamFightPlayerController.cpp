// Fill out your copyright notice in the Description page of Project Settings.


#include "Management/TeamFightPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Management/NetworkGameInstance.h"
#include "Management/TeamFightGameMode.h"
#include "Management/TeamFightPlayerState.h"
#include "UI/TeamFightNoticeUI.h"

void ATeamFightPlayerController::ShowNoticeUI(const FString& message, float duration)
{
	if (NoticeUIWidget)
	{
		NoticeUIWidget->ShowNotice(message, duration);
	}
}

ATeamFightPlayerController::ATeamFightPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	{
		ConstructorHelpers::FClassFinder<UTeamFightNoticeUI> tempWidget(
			TEXT(
				"'/Game/DuoShooting/UIs/WBP_TeamFightNoticeUI.WBP_TeamFightNoticeUI_C'"));
		if (tempWidget.Succeeded()) { NoticeUIFactory = tempWidget.Class; }
	}
}

void ATeamFightPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (NoticeUIFactory)
	{
		NoticeUIWidget = CreateWidget<UTeamFightNoticeUI>(GetWorld(), NoticeUIFactory);

		if (NoticeUIWidget != nullptr)
		{
			NoticeUIWidget->AddToViewport();
		}
	}

	// 서버에게 유저 이름을 알린다
	if (UNetworkGameInstance* netGameInstance = Cast<UNetworkGameInstance>(GetWorld()->GetGameInstance()))
	{
		ServerRPC_SendUserName(netGameInstance->GetUserName());
	}
}

void ATeamFightPlayerController::ServerRPC_SendUserName_Implementation(const FString& UserName)
{
	// 서버는 클라이언트로부터 유저이름을 받아 플레이어 스테이트에 저장
	if (ATeamFightPlayerState* teamFightPlayerState = GetPlayerState<ATeamFightPlayerState>())
	{
		teamFightPlayerState->SetUserName(UserName);
	}
}

void ATeamFightPlayerController::ServerRPC_RequestGameEnterNotice_Implementation()
{
	if (ATeamFightGameMode* teamFightGameMode = Cast<ATeamFightGameMode>(GetWorld()->GetAuthGameMode()))
	{
		int32 targetScore = teamFightGameMode->GetTargetWinScore();
		FString notice = FString::Printf(TEXT("먼저 %d점을 모은 팀이 승리"), targetScore);
		ClientRPC_ShowNotice(notice, 4.0f, 1.0f);
	}
}

void ATeamFightPlayerController::ClientRPC_ShowNotice_Implementation(const FString& message, float duration, float wait)
{
	if (wait > 0.0f)
	{
		FTimerHandle TimerHandle;
		
		GetWorldTimerManager().SetTimer(
			TimerHandle,
			[this, message, duration]()
			{
				ShowNoticeUI(message, duration);
			},
			wait,
			false
		);
	}
	else
	{
		ShowNoticeUI(message, duration);
	}
}
