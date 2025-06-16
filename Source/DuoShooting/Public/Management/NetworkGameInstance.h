// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetworkGameInstance.generated.h"

//세션 정보 구조체
USTRUCT()
struct FSessionInfo
{
	GENERATED_BODY()

	//방 이름
	FString RoomName;
	//호스트 유저 이름
	FString HostUserName;
	//최대 플레이어 수
	int32 MaxPlayerCount;
	//현재 세션 입장 플레이어 수
	int32 CurrentPlayerCount;
	//최소 시작 플레이어 수
	int32 StartPlayerCount;

	//핑
	int32 PingSpeed;
	
	//세션 정보 식별 아이디 인덱스
	int32 ID_Index;
};

//세션 검색 모두 끝났을 때 호출될 델리게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FFindAndSetExistedSessionsComplete, const TArray<FSessionInfo>&);

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API UNetworkGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	virtual void Init() override;
	
	//==변수==
private:
	//세션 인터페이스
	IOnlineSessionPtr SessionInterface;

	//사용자 이름
	FString UserName;
	
	//세션(방) 이름
	FString RoomName;

	//세션 검색 설정 및 결과 정보 보관
	TSharedPtr<FOnlineSessionSearch> ExistedSessionSearch;

	//최대 검색 세션 수
	int32 MaxSearchResultsNum = 10;

	//현재 검색된 모든 세션 정보 (세션 입장 시 전부 지워두기 (굳이 가지고 있을 필요 없음))
	TArray<FSessionInfo> CurrentFindSessions;

	//시작 인원 수
	int32 StartPlayerCount;

public:
	//세션 검색 후 해야할 일 이벤트 콜백용 델리게이트
	FFindAndSetExistedSessionsComplete OnFindAndSetExistedSessionsCompleteDelegate;
	
	//==함수==
private:
public:
	void SetUserName(FString newUserName);

	//검색된 세션 정보 가져오기. 메모리 최적화 및 세부 내용 건들이지 않도록 방지!
	const TArray<FSessionInfo>& GetSessionInfos();

	//세션 생성
	void CreateNewSession(FString roomName, int32 maxPlayerCount, int32 startPlayercount);

	//세션 생성이 성공적으로 완료되면 호출되는 이벤트 콜백
	UFUNCTION()
	void OnCreateNewSessionComplete(FName roomName, bool bSuccessful);

	//세션 찾기
	void FindExistedSessions();

	//세션 찾기가 성공적으로 완료되면 호출되는 이벤트 콜백
	void OnFindExistedSessionsComplete(bool bSuccessful);

	//세션에 입장하고 싶을 때
	void JoinExistedSessions(int32 id_Index);
	//세션 입장 후 할 일 (실제 맵 이동과 같은 일들)
	void OnJoinExistedSessionsComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);

};
