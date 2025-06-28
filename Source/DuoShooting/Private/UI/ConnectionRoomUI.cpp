// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ConnectionRoomUI.h"

#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/WidgetSwitcher.h"
#include "Management/NetworkGameInstance.h"

void UConnectionRoomUI::NativeConstruct()
{
	Super::NativeConstruct();

	NetGameInstance = GetGameInstance<UNetworkGameInstance>();
	if (nullptr == NetGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Not exist proper gameinstance!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
	}

	//버튼 배열 등록
	TArray_Button_MaxPlayerCount.Add(Button_MaxPlayerCount_2);
	TArray_Button_MaxPlayerCount.Add(Button_MaxPlayerCount_4);
	TArray_Button_MaxPlayerCount.Add(Button_MaxPlayerCount_6);
	TArray_Button_MaxPlayerCount.Add(Button_MaxPlayerCount_8);
	TArray_Button_MaxPlayerCount.Add(Button_MaxPlayerCount_10);
	TArray_Button_MaxPlayerCount.Add(Button_MaxPlayerCount_12);

	TArray_Button_StartPlayerCount.Add(Button_StartPlayerCount_2);
	TArray_Button_StartPlayerCount.Add(Button_StartPlayerCount_4);
	TArray_Button_StartPlayerCount.Add(Button_StartPlayerCount_6);
	TArray_Button_StartPlayerCount.Add(Button_StartPlayerCount_8);
	TArray_Button_StartPlayerCount.Add(Button_StartPlayerCount_10);
	TArray_Button_StartPlayerCount.Add(Button_StartPlayerCount_12);

	//유저 정보 페이지 . 기능 등록
	TextEdit_UserName->OnTextChanged.AddDynamic(this, &UConnectionRoomUI::OnUserNameInput);
	Button_Register->OnClicked.AddDynamic(this, &UConnectionRoomUI::OnClickedRegisterButton);

	//최대 인원수 기능 등록
	Button_MaxPlayerCount_2->OnClicked.AddDynamic(this, &UConnectionRoomUI::OnClickedMaxPlayerCountButton2);
	Button_MaxPlayerCount_4->OnClicked.AddDynamic(this, &UConnectionRoomUI::OnClickedMaxPlayerCountButton4);
	Button_MaxPlayerCount_6->OnClicked.AddDynamic(this, &UConnectionRoomUI::OnClickedMaxPlayerCountButton6);
	Button_MaxPlayerCount_8->OnClicked.AddDynamic(this, &UConnectionRoomUI::OnClickedMaxPlayerCountButton8);
	Button_MaxPlayerCount_10->OnClicked.AddDynamic(this, &UConnectionRoomUI::OnClickedMaxPlayerCountButton10);
	Button_MaxPlayerCount_12->OnClicked.AddDynamic(this, &UConnectionRoomUI::OnClickedMaxPlayerCountButton12);

	//최소 시작 인원수 기능 등록
	Button_StartPlayerCount_2->OnClicked.AddDynamic(this, &UConnectionRoomUI::OnClickedStartPlayerCountButton2);
	Button_StartPlayerCount_4->OnClicked.AddDynamic(this, &UConnectionRoomUI::OnClickedStartPlayerCountButton4);
	Button_StartPlayerCount_6->OnClicked.AddDynamic(this, &UConnectionRoomUI::OnClickedStartPlayerCountButton6);
	Button_StartPlayerCount_8->OnClicked.AddDynamic(this, &UConnectionRoomUI::OnClickedStartPlayerCountButton8);
	Button_StartPlayerCount_10->OnClicked.AddDynamic(this, &UConnectionRoomUI::OnClickedStartPlayerCountButton10);
	Button_StartPlayerCount_12->OnClicked.AddDynamic(this, &UConnectionRoomUI::OnClickedStartPlayerCountButton12);

	//인원수 미리 설정
	OnClickedMaxPlayerCountButton12();
	OnClickedStartPlayerCountButton12();

	//방만들기 버튼 기능 등록
	Button_CreateRoom->OnClicked.AddDynamic(this, &UConnectionRoomUI::OnClickedCreateRoomButton);

	NetGameInstance->CreateAndJoinRoomFailedDelegate.AddUObject(this, &UConnectionRoomUI::RemoveCreateLoadingUI);
}

void UConnectionRoomUI::OnUserNameInput(const FText& text)
{
	FString Input = text.ToString();

	//글자수를 판단하는데, 한글의 글자수도 적절히 판단해 준다.
	if (Input.Len() > MaxCharactersNum)
	{
		//왼쪽의 글자를 최대 글자 수만큼만 가져온다.
		Input = Input.Left(MaxCharactersNum);
		//그것만 적용시킨다.
		TextEdit_UserName->SetText(FText::FromString(Input));
	}
}

void UConnectionRoomUI::OnClickedRegisterButton()
{
	FString userName = TextEdit_UserName->GetText().ToString();
	if (userName.Len() <= 0)
		return;
	NetGameInstance->SetUserName(TextEdit_UserName->GetText().ToString());
	WidgetSwitcher->SetActiveWidgetIndex(1);
}

#pragma region 인원수 버튼 별 반응 함수 모음
void UConnectionRoomUI::OnClickedMaxPlayerCountButton2() { SetMaxPlayerCount(0); }
void UConnectionRoomUI::OnClickedMaxPlayerCountButton4() { SetMaxPlayerCount(1); }
void UConnectionRoomUI::OnClickedMaxPlayerCountButton6() { SetMaxPlayerCount(2); }
void UConnectionRoomUI::OnClickedMaxPlayerCountButton8() { SetMaxPlayerCount(3); }
void UConnectionRoomUI::OnClickedMaxPlayerCountButton10() { SetMaxPlayerCount(4); }
void UConnectionRoomUI::OnClickedMaxPlayerCountButton12() { SetMaxPlayerCount(5); }

void UConnectionRoomUI::OnClickedStartPlayerCountButton2() { SetStartPlayerCount(0); }
void UConnectionRoomUI::OnClickedStartPlayerCountButton4() { SetStartPlayerCount(1); }
void UConnectionRoomUI::OnClickedStartPlayerCountButton6() { SetStartPlayerCount(2); }
void UConnectionRoomUI::OnClickedStartPlayerCountButton8() { SetStartPlayerCount(3); }
void UConnectionRoomUI::OnClickedStartPlayerCountButton10() { SetStartPlayerCount(4); }
void UConnectionRoomUI::OnClickedStartPlayerCountButton12() { SetStartPlayerCount(5); }
#pragma endregion

void UConnectionRoomUI::SetMaxPlayerCount(int32 newMaxPlayerCountIndex)
{
	UnSelectButtonDesign(TArray_Button_MaxPlayerCount[MaxPlayerCountIndex]);
	MaxPlayerCountIndex = newMaxPlayerCountIndex;
	SelectButtonDesign(TArray_Button_MaxPlayerCount[MaxPlayerCountIndex]);

	for (int i = 0; i <= MaxPlayerCountIndex; i++)
	{
		TArray_Button_StartPlayerCount[i]->SetIsEnabled(true);
	}
	for (int i = MaxPlayerCountIndex + 1; i < TArray_Button_StartPlayerCount.Num(); i++)
	{
		TArray_Button_StartPlayerCount[i]->SetIsEnabled(false);
	}
	
	if (StartPlayerCountIndex > MaxPlayerCountIndex)
	{
		SetStartPlayerCount(MaxPlayerCountIndex);
	}
}

void UConnectionRoomUI::SetStartPlayerCount(int32 newStartPlayerCountIndex)
{
	UnSelectButtonDesign(TArray_Button_StartPlayerCount[StartPlayerCountIndex]);
	StartPlayerCountIndex = newStartPlayerCountIndex;
	SelectButtonDesign(TArray_Button_StartPlayerCount[StartPlayerCountIndex]);
}

void UConnectionRoomUI::SelectButtonDesign(UButton* button)
{
	button->SetRenderScale(SelectedButtonSize);
	button->SetColorAndOpacity(SelectedButtonColor);
}

void UConnectionRoomUI::UnSelectButtonDesign(UButton* button)
{
	button->SetRenderScale(FVector2D::One());
	button->SetColorAndOpacity(FLinearColor::White);
}

void UConnectionRoomUI::OnClickedCreateRoomButton()
{
	//로딩 UI 생성
	CreateLoadingUI = CreateWidget(GetWorld(), OriginCreateLoadingUI);
	CreateLoadingUI->AddToViewport(1);
	
	FString roomName = TextEdit_RoomName->GetText().ToString();
	int32 maxPlayerCount = (MaxPlayerCountIndex + 1) * 2;
	int32 startPlayerCount = (StartPlayerCountIndex + 1) * 2;
	NetGameInstance->CreateNewSession(roomName, maxPlayerCount, startPlayerCount);
}

void UConnectionRoomUI::RemoveCreateLoadingUI()
{
	if (CreateLoadingUI == nullptr)
		return;
	CreateLoadingUI->RemoveFromParent();
	CreateLoadingUI = nullptr;
}
