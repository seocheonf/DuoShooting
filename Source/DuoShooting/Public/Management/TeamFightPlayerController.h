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

	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowNotice(const FString& message, float duration, float wait = -1.0f);

	UFUNCTION(Server, Reliable)
	void ServerRPC_RequestGameEnterNotice();
};
