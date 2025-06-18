// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TeamFightNoticeUI.h"

#include "Components/TextBlock.h"

void UTeamFightNoticeUI::HideNotice()
{
	if (HideAnim) { PlayAnimation(HideAnim); }
}

void UTeamFightNoticeUI::NativeDestruct()
{
	Super::NativeDestruct();
	GetWorld()->GetTimerManager().ClearTimer(HideNoticeHandle);
}

void UTeamFightNoticeUI::ShowNotice(const FString& message, float duration)
{
	if (Message) { Message->SetText(FText::FromString(message)); }
	if (ShowAnim) { PlayAnimation(ShowAnim); }

	GetWorld()->GetTimerManager().ClearTimer(HideNoticeHandle);
	GetWorld()->GetTimerManager().SetTimer(HideNoticeHandle, this, &UTeamFightNoticeUI::HideNotice,
	duration, false);
}
