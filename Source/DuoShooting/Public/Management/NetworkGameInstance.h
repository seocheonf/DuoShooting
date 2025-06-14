// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetworkGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API UNetworkGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	virtual void Init() override;
	
	//==변수==//
private:
	//사용자 이름
	FString UserName;
	//세션(방) 이름
	FString RoomName;
	
	IOnlineSessionPtr SessionInterface;

	//==함수==//
private:
public:
	void SetUserName(FString newUserName);

	//세션 생성
	void CreateNewSession(FString roomName, int32 maxPlayerCount, int32 startPlayercount);

	//세션 생성이 성공적으로 완료되면 호출되는 이벤트 콜백
	UFUNCTION()
	void OnCreateNewSessionComplete(FName roomName, bool bSuccessful);
	
};
