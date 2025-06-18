// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ConnectionRoomFindSessionUI.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/ScrollBox.h"
#include "Management/NetworkGameInstance.h"
#include "UI/SessionListElementUI.h"

void UConnectionRoomFindSessionUI::NativeConstruct()
{
	Super::NativeConstruct();

	NetGameInstance = GetGameInstance<UNetworkGameInstance>();
	if (nullptr == NetGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Not exist proper gameinstance!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
	}
	
	//방 찾기 버튼 기능 등록
	Button_FindRoom->OnClicked.AddDynamic(this, &UConnectionRoomFindSessionUI::OnClickedFindRoomButton);
	
	//현재 세션을 찾아서 업데이트한 후 호출되는 콜백 함수에서 할 일 등록
	NetGameInstance->OnFindAndSetExistedSessionsCompleteDelegate.AddUObject(this, &UConnectionRoomFindSessionUI::OnUpdateExistedSessions);

	//입장 실패시 입장 로딩 UI를 빼자.
	NetGameInstance->CreateAndJoinRoomFailedDelegate.AddUObject(this, &UConnectionRoomFindSessionUI::SetOffEnterLoadingUI);
}

void UConnectionRoomFindSessionUI::OnClickedFindRoomButton()
{
	//화면 어둡게
	Canvas_Loading->SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(AnimLoadingIcon, 0, 0);
	
	//세션 찾기 요청
	NetGameInstance->FindExistedSessions();
}

void UConnectionRoomFindSessionUI::OnUpdateExistedSessions(const TArray<FSessionInfo>& newSessionInfos)
{
	//스크롤 박스를 비우고
	ScrollBox_SessionList->ClearChildren();
	for (auto each : newSessionInfos)
	{
		//스크롤 박스에 들어갈 엘리멘트를 생성
		USessionListElementUI* eachElement = CreateWidget<USessionListElementUI>(this, OriginSessionListElement);
		//엘리멘트 초기화
		eachElement->Set(each, this);
		//스크롤 박스에 엘리멘트 부착
		ScrollBox_SessionList->AddChild(eachElement);
	}

	//화면 밝게
	StopAnimation(AnimLoadingIcon);
	Canvas_Loading->SetVisibility(ESlateVisibility::Hidden);
	
}

void UConnectionRoomFindSessionUI::SetOnEnterLoadingUI()
{
	WBP_MapLoading->SetVisibility(ESlateVisibility::Visible);
	//Canvas_Entering->SetVisibility(ESlateVisibility::Visible);
	//PlayAnimation(AnimLoadingIconEnter, 0, 0);
}

void UConnectionRoomFindSessionUI::SetOffEnterLoadingUI()
{
	WBP_MapLoading->SetVisibility(ESlateVisibility::Hidden);
	//Canvas_Entering->SetVisibility(ESlateVisibility::Visible);
	//StopAnimation(AnimLoadingIconEnter);

	NetGameInstance->CreateAndJoinRoomCompleteDelegate.Clear();
}

void UConnectionRoomFindSessionUI::SetOnEnterUI()
{
	WBP_MapLoading->SetVisibility(ESlateVisibility::Visible);
}
