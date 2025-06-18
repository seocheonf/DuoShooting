// Fill out your copyright notice in the Description page of Project Settings.


#include "Management/TeamFightPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Management/TeamFightGameMode.h"
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
