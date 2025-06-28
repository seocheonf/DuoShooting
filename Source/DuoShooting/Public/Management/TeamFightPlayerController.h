// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TeamFightPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API ATeamFightPlayerController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UTeamFightNoticeUI> NoticeUIFactory;

	UPROPERTY()
	class UTeamFightNoticeUI* NoticeUIWidget;

	void ShowNoticeUI(const FString& message, float duration);

public:
	ATeamFightPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void BeginPlay() override;

	// 클라이언트들에게 메시지를 보이게 한다
	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowNotice(const FString& message, float duration, float wait = -1.0f);

	// 클라이언트가 서버에게 메시지를 요청한다
	UFUNCTION(Server, Reliable)
	void ServerRPC_RequestGameEnterNotice();

	// 클라이언트 -> 서버로 유저이름을 알려준다
	UFUNCTION(Server, Reliable)
	void ServerRPC_SendUserName(const FString& UserName);
};
