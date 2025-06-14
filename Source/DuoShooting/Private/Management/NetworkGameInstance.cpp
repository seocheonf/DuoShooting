// Fill out your copyright notice in the Description page of Project Settings.


#include "Management/NetworkGameInstance.h"
#include "OnlineSessionSettings.h"


void UNetworkGameInstance::Init()
{
	Super::Init();

	if (auto onlineSubSystem = IOnlineSubsystem::Get())
	{
		//온라인 서브시스템 가져와서, 통신 처리를 위한 인터페이스를 보관해두기.
		SessionInterface = onlineSubSystem->GetSessionInterface();

		//세션 이벤트 콜백 등록
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetworkGameInstance::OnCreateNewSessionComplete);
	}

	
}

void UNetworkGameInstance::SetUserName(FString newUserName)
{
	UserName = newUserName;
}

void UNetworkGameInstance::CreateNewSession(FString roomName, int32 maxPlayerCount, int32 startPlayercount)
{
	//세션 설정
	FOnlineSessionSettings sessionSettings;

	//데디케이드 서버 접속 설정
	sessionSettings.bIsDedicated = false;

	//로컬 서버 접속 설정
	sessionSettings.bIsLANMatch = true;

	//매칭 노출 여부
	sessionSettings.bShouldAdvertise = true;

	//온라인 상태 정보 활용 허가 여부
	sessionSettings.bUsesPresence = true;

	//로비 사용 여부
	sessionSettings.bUseLobbiesIfAvailable = true;

	//게임 진행 중 참여 허가 여부
	sessionSettings.bAllowJoinViaPresence = true;
	sessionSettings.bAllowJoinInProgress = true;

	//최대 인원수
	sessionSettings.NumPublicConnections = maxPlayerCount;

	//최소 시작 인원수
	sessionSettings.Set(FName("StartPlayerCount"), startPlayercount, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//호스트 네임 설정
	sessionSettings.Set(FName("HostName"), UserName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//방 이름 설정
	RoomName = roomName;
	sessionSettings.Set(FName("RoomName"), RoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	//NetID 가져오기
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
	
	SessionInterface->CreateSession(*netID, FName(RoomName), sessionSettings);
	
}

void UNetworkGameInstance::OnCreateNewSessionComplete(FName roomName, bool bSuccessful)
{
	//세션 생성 및 접속에 성공했다면
	if (bSuccessful)
	{
		//listen 서버로 열면서, 맵을 이동하겠다!
		GetWorld()->ServerTravel(TEXT("/Game/DuoShooting/Maps/KHM/L_KMH_TeamFight?Listen?port=7777"));
	}
}
