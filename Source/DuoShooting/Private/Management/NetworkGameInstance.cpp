// Fill out your copyright notice in the Description page of Project Settings.


#include "Management/NetworkGameInstance.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"


void UNetworkGameInstance::Init()
{
	Super::Init();

	if (auto onlineSubSystem = IOnlineSubsystem::Get())
	{
		//온라인 서브시스템 가져와서, 통신 처리를 위한 인터페이스를 보관해두기.
		SessionInterface = onlineSubSystem->GetSessionInterface();

		//세션 생성 이벤트 콜백 등록
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetworkGameInstance::OnCreateNewSessionComplete);

		//세션 검색 이벤트 콜백 등록
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetworkGameInstance::OnFindExistedSessionsComplete);

		//세션 입장 이벤트 콜백 등록
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNetworkGameInstance::OnJoinExistedSessionsComplete);
	}

	
}

void UNetworkGameInstance::SetUserName(FString newUserName)
{
	UserName = newUserName;
}

const TArray<FSessionInfo>& UNetworkGameInstance::GetSessionInfos()
{
	return CurrentFindSessions;
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
	StartPlayerCount = startPlayercount;
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
		//검색을 위해 가지고 있던 정보들을 비워주자.
		CurrentFindSessions.Empty();
		//listen 서버로 열면서, 맵을 이동하겠다!
		GetWorld()->ServerTravel(TEXT("/Game/DuoShooting/Maps/KHM/L_KMH_TeamFight?Listen?port=7777"));
	}
}

void UNetworkGameInstance::FindExistedSessions()
{
	//세션 검색 조건 설정 데이터 생성 (TSharedPtr)
	//나중에 이 값을 TSharedRef로 보내줘야 하는 부분 때문인 것으로 추정.
	ExistedSessionSearch = MakeShareable(new FOnlineSessionSearch());

	//검색 조건 설정 (존재 여부 검색 허용한 대상만 찾기)
	ExistedSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	//Lan 사용 여부
	ExistedSessionSearch->bIsLanQuery = true;

	//최대 검색 세션 수
	ExistedSessionSearch->MaxSearchResults = MaxSearchResultsNum;

	//세션 검색 요청
	SessionInterface->FindSessions(0, ExistedSessionSearch.ToSharedRef());
}

void UNetworkGameInstance::OnFindExistedSessionsComplete(bool bSuccessful)
{
	//세션 검색 실패 시 어느 일도 하지 말기
	if (false == bSuccessful)
		return;

	//기존 검색 세션 초기화
	CurrentFindSessions.Empty();
	
	//세션 검색 결과를 TArray로 받아오기
	auto results = ExistedSessionSearch->SearchResults;

	for (int i = 0; i<results.Num(); i++)
	{
		auto eachResult = results[i];

		//검색 결과가 유효하지 않다면 다음 검색 결과로 넘어간다.
		if (false == eachResult.IsValid())
		{
			continue;
		}

		//세션 정보 데이터 저장 구조체 마련
		FSessionInfo sessionInfo;

		//세션 정보 값 가져와서 저장.
		//방 이름
		eachResult.Session.SessionSettings.Get(FName("RoomName"), sessionInfo.RoomName);
		//호스트 유저 이름
		eachResult.Session.SessionSettings.Get(FName("HostName"), sessionInfo.HostUserName);
		//최대 플레이어 수
		int32 maxPlayerCount = eachResult.Session.SessionSettings.NumPublicConnections;
		sessionInfo.MaxPlayerCount = maxPlayerCount;
		//현재 입장한 플레이어 수 (최대 플레이어 수 - 남은 입장 인원 수)
		sessionInfo.CurrentPlayerCount = maxPlayerCount - eachResult.Session.NumOpenPublicConnections;
		//최소 시작 플레이어 수
		eachResult.Session.SessionSettings.Get(FName("StartPlayerCount"), sessionInfo.StartPlayerCount);

		//핑
		sessionInfo.PingSpeed = eachResult.PingInMs;
		
		//식별용 Index
		sessionInfo.ID_Index = i;
		
		CurrentFindSessions.Add(sessionInfo);
	}
	
	OnFindAndSetExistedSessionsCompleteDelegate.Broadcast(CurrentFindSessions);
}

void UNetworkGameInstance::JoinExistedSessions(int32 id_Index)
{
	//id_index를 기반으로 어떤 세션이었는지 찾기
	auto searchResult = ExistedSessionSearch->SearchResults[id_Index];
	//들어갈 세션 방 정보 받아오기
	searchResult.Session.SessionSettings.Get(FName("RoomName"), RoomName);

	searchResult.Session.SessionSettings.Get(FName("StartPlayerCount"), StartPlayerCount);

	//실제 방으로 들어가기s
	SessionInterface->JoinSession(0, FName(RoomName), searchResult);
}

void UNetworkGameInstance::OnJoinExistedSessionsComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	//만약 잘 접속 했다면
	if (EOnJoinSessionCompleteResult::Success == result)
	{
		APlayerController* playerController = GetWorld()->GetFirstPlayerController();
		FString url;
		//세션 호스트가 ServerTravle로 이동한 정보를 가져오는 것으로 추정
		SessionInterface->GetResolvedConnectString(sessionName, url);
		if (false == url.IsEmpty())
		{
			//언리얼 폴더상 절대 좌표로 이동.
			playerController->ClientTravel(url, TRAVEL_Absolute);
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("JoinSessionComplete Failed : %d"), result);
	}
}

int32 UNetworkGameInstance::GetStartPlayerCount()
{
	return StartPlayerCount;
}

