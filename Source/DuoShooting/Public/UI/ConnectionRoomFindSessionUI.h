// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConnectionRoomFindSessionUI.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API UConnectionRoomFindSessionUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
	//==변수==
private:
	//세션 요청용 게임 인스턴스
	class UNetworkGameInstance* NetGameInstance;
	
	//방 찾기 버튼
	UPROPERTY(meta = (BindWidget))		
	class UButton* Button_FindRoom;

	//찾은 방 목록
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ScrollBox_SessionList;

	//찾은 방 목록에 들어갈 UI 원본
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<class USessionListElementUI> OriginSessionListElement;

	//==함수==
private:
	
	//방 찾기 버튼 클릭 시 해야 할 일
	UFUNCTION()
	void OnClickedFindRoomButton();
	
	//새로운 세션 정보가 들어왔을 때, 바인딩 되어 콜백으로 실행되는 이벤트 함수
	//세션 목록 UI를 갱신한다.
	UFUNCTION()
	void OnUpdateExistedSessions(const TArray<struct FSessionInfo>& newSessionInfos);
	
public:
};
