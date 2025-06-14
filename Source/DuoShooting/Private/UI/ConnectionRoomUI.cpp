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
	
	TextEdit_UserName->OnTextChanged.AddDynamic(this, &UConnectionRoomUI::OnUserNameInput);
	Button_Register->OnClicked.AddDynamic(this, &UConnectionRoomUI::OnClickedRegisterButton);
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
	NetGameInstance->SetUserName(TextEdit_UserName->GetText().ToString());
	WidgetSwitcher->SetActiveWidgetIndex(1);
}