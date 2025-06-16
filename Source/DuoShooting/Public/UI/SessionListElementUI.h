// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionListElementUI.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API USessionListElementUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
	//==변수==
private:

	class UNetworkGameInstance* NetGameInstance;
	
	//세션 아이디 : NetworkGameInstance에서 세션 검색 결과를 배열로 다루고 있는데, 이를 찾기위한 아이디이자 인덱스 
	int32 SessionID_Index;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_RoomName;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_HostName;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_PlayerCount;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_StartPlayerCount;
	//핑 UI	
	UPROPERTY(meta = (BindWidget))
	class UPingUI* PingUI;
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Join;

	//==함수==

private:
	//입장 버튼 누를 시, 세션 및 맵 입장.
	UFUNCTION()
	void JoinExistedSession();
public:
	//기본 값을 설정하는 함수. 초기화 함수 정도로 생각하면 좋음
	void Set(const struct FSessionInfo& sessionInfo);
	
};
