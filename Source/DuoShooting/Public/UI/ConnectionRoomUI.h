// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConnectionRoomUI.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API UConnectionRoomUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
	//==바인딩 변수==
private:

	//커넥션 화면 전환용
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;

	//유저정보 입력화면
	UPROPERTY(meta = (BindWidget))
	class UEditableText* TextEdit_UserName;
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Register;
	
public:

	//==기타 변수==
private:
	int32 MaxCharactersNum = 12;

	class UNetworkGameInstance* NetGameInstance;
	
	//==함수==
private:
	//유저 이름 제한
	UFUNCTION()
	void OnUserNameInput(const FText& text);

	//유저 이름 등록
	UFUNCTION()
	void OnClickedRegisterButton();
	
};
