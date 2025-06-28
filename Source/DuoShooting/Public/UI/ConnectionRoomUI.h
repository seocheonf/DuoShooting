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
	//유저 이름 설정
	UPROPERTY(meta = (BindWidget))
	class UEditableText* TextEdit_UserName;
	//유저 이름 등록
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Register;

	//방 생성 및 방 찾기 화면
	//방 생성 및 접속 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_CreateRoom;
	//방 이름 설정
	UPROPERTY(meta = (BindWidget))
	class UEditableText* TextEdit_RoomName;
	//최대 인원수 버튼 목록
	TArray<class UButton*> TArray_Button_MaxPlayerCount;
	//최소 시작 인원수 버튼 목록
	TArray<class UButton*> TArray_Button_StartPlayerCount;
#pragma region 최대 인원수 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_MaxPlayerCount_2;
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_MaxPlayerCount_4;
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_MaxPlayerCount_6;
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_MaxPlayerCount_8;
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_MaxPlayerCount_10;
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_MaxPlayerCount_12;
#pragma endregion
#pragma region 최소 시작 인원수 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_StartPlayerCount_2;
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_StartPlayerCount_4;
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_StartPlayerCount_6;
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_StartPlayerCount_8;
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_StartPlayerCount_10;
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_StartPlayerCount_12;
#pragma endregion
	
public:

	//==기타 변수==
private:
	//세션 연결 요청을 위한 게임 인스턴스
	class UNetworkGameInstance* NetGameInstance;
	
	//최대 유저 이름 글자 수
	int32 MaxCharactersNum = 12;

	//선택된 버튼 조절 크기
	FVector2D SelectedButtonSize = FVector2D(1.25f, 1.25f);
	//선택된 버튼 조절 색상
	FLinearColor SelectedButtonColor = FLinearColor::Black;

	//최소 인원 수
	int32 StartPlayerCountIndex = 0;
	//최대 인원 수
	int32 MaxPlayerCountIndex = 0;
	
	//==함수==
private:
	//유저 이름 제한
	UFUNCTION()
	void OnUserNameInput(const FText& text);

	//유저 이름 등록
	UFUNCTION()
	void OnClickedRegisterButton();

	//최대 인원수 버튼 클릭
	UFUNCTION()
	void OnClickedMaxPlayerCountButton2();
	UFUNCTION()
	void OnClickedMaxPlayerCountButton4();
	UFUNCTION()
	void OnClickedMaxPlayerCountButton6();
	UFUNCTION()
	void OnClickedMaxPlayerCountButton8();
	UFUNCTION()
	void OnClickedMaxPlayerCountButton10();
	UFUNCTION()
	void OnClickedMaxPlayerCountButton12();

	//최소 시작 인원수 버튼 클릭
	UFUNCTION()
	void OnClickedStartPlayerCountButton2();
	UFUNCTION()
	void OnClickedStartPlayerCountButton4();
	UFUNCTION()
	void OnClickedStartPlayerCountButton6();
	UFUNCTION()
	void OnClickedStartPlayerCountButton8();
	UFUNCTION()
	void OnClickedStartPlayerCountButton10();
	UFUNCTION()
	void OnClickedStartPlayerCountButton12();
	
	//최대 인원수 버튼에 따른 최대 인원수 값 설정
	void SetMaxPlayerCount(int32 newMaxPlayerCountIndex);
	
	//최소 시작 인원수 버튼에 따른 최소 인원수 값 설정
	void SetStartPlayerCount(int32 newStartPlayerCountIndex);

	//선택된 버튼 디자인
	void SelectButtonDesign(UButton* button);
	//해제된 버튼 디자인
	void UnSelectButtonDesign(UButton* button);

	//방 만들기 버튼 클릭 시 해야 할 일
	UFUNCTION()
	void OnClickedCreateRoomButton();


	//===방생성 로딩 UI
	//생성 로딩 위젯
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> OriginCreateLoadingUI;
	class UUserWidget* CreateLoadingUI;

private:
	//생성 로딩 위젯 제거
	UFUNCTION()
	void RemoveCreateLoadingUI();
};
