// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SessionListElementUI.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Management/NetworkGameInstance.h"
#include "UI/PingUI.h"

void USessionListElementUI::NativeConstruct()
{
	Super::NativeConstruct();

	NetGameInstance = GetGameInstance<UNetworkGameInstance>();
	if (nullptr == NetGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Not exist proper gameinstance!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
	}

	Button_Join->OnClicked.AddDynamic(this, &USessionListElementUI::JoinExistedSession);
}

void USessionListElementUI::JoinExistedSession()
{
	NetGameInstance->JoinExistedSessions(SessionID_Index);
}

void USessionListElementUI::Set(const FSessionInfo& sessionInfo)
{
	Text_RoomName->SetText(FText::FromString(sessionInfo.RoomName));
	Text_HostName->SetText(FText::FromString(sessionInfo.HostUserName));
	FString playerCount = FString::Printf(TEXT("%d / %d"),sessionInfo.CurrentPlayerCount, sessionInfo.MaxPlayerCount);
	Text_PlayerCount->SetText(FText::FromString(playerCount));
	Text_StartPlayerCount->SetText(FText::FromString(FString::Printf(TEXT("%d"), sessionInfo.StartPlayerCount)));
	PingUI->SetPingUI(sessionInfo.PingSpeed);
	
	SessionID_Index = sessionInfo.ID_Index;
}
