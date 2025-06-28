// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PingUI.h"

#include "Components/Image.h"

void UPingUI::SetPingUI(float currentPing)
{
	if (currentPing < 30)
	{
		Image_Ping1->SetColorAndOpacity(FLinearColor::Green);
		Image_Ping2->SetColorAndOpacity(FLinearColor::Green);
		Image_Ping3->SetColorAndOpacity(FLinearColor::Green);
		Image_Ping4->SetColorAndOpacity(FLinearColor::Green);
		Image_Ping5->SetColorAndOpacity(FLinearColor::Green);

		Image_Ping1->SetVisibility(ESlateVisibility::Visible);
		Image_Ping2->SetVisibility(ESlateVisibility::Visible);
		Image_Ping3->SetVisibility(ESlateVisibility::Visible);
		Image_Ping4->SetVisibility(ESlateVisibility::Visible);
		Image_Ping5->SetVisibility(ESlateVisibility::Visible);

		return;
	}
	if (currentPing < 60)
	{
		Image_Ping1->SetColorAndOpacity(FLinearColor::Green);
		Image_Ping2->SetColorAndOpacity(FLinearColor::Green);
		Image_Ping3->SetColorAndOpacity(FLinearColor::Green);
		Image_Ping4->SetColorAndOpacity(FLinearColor::Green);

		Image_Ping1->SetVisibility(ESlateVisibility::Visible);
		Image_Ping2->SetVisibility(ESlateVisibility::Visible);
		Image_Ping3->SetVisibility(ESlateVisibility::Visible);
		Image_Ping4->SetVisibility(ESlateVisibility::Visible);
		Image_Ping5->SetVisibility(ESlateVisibility::Hidden);

		return;
	}
	if (currentPing < 120)
	{
		Image_Ping1->SetColorAndOpacity(FLinearColor::Yellow);
		Image_Ping2->SetColorAndOpacity(FLinearColor::Yellow);
		Image_Ping3->SetColorAndOpacity(FLinearColor::Yellow);

		Image_Ping1->SetVisibility(ESlateVisibility::Visible);
		Image_Ping2->SetVisibility(ESlateVisibility::Visible);
		Image_Ping3->SetVisibility(ESlateVisibility::Visible);
		Image_Ping4->SetVisibility(ESlateVisibility::Hidden);
		Image_Ping5->SetVisibility(ESlateVisibility::Hidden);

		return;
	}
	if (currentPing < 240)
	{
		Image_Ping1->SetColorAndOpacity(FLinearColor::Yellow);
		Image_Ping2->SetColorAndOpacity(FLinearColor::Yellow);

		Image_Ping1->SetVisibility(ESlateVisibility::Visible);
		Image_Ping2->SetVisibility(ESlateVisibility::Visible);
		Image_Ping3->SetVisibility(ESlateVisibility::Hidden);
		Image_Ping4->SetVisibility(ESlateVisibility::Hidden);
		Image_Ping5->SetVisibility(ESlateVisibility::Hidden);

		return;
	}
	
	{
		Image_Ping1->SetColorAndOpacity(FLinearColor::Red);

		Image_Ping1->SetVisibility(ESlateVisibility::Visible);
		Image_Ping2->SetVisibility(ESlateVisibility::Hidden);
		Image_Ping3->SetVisibility(ESlateVisibility::Hidden);
		Image_Ping4->SetVisibility(ESlateVisibility::Hidden);
		Image_Ping5->SetVisibility(ESlateVisibility::Hidden);
	}
	
}
