// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamFightNoticeUI.generated.h"

/** 배틀 진행 안내 메시지용 UI
 * 
 */
UCLASS()
class DUOSHOOTING_API UTeamFightNoticeUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Message;

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowAnim;

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	class UWidgetAnimation* HideAnim;


	FTimerHandle HideNoticeHandle;

	void HideNotice();

	virtual void NativeDestruct() override;

public:
	void ShowNotice(const FString& message, float duration);
};
