// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UHealthBarWidget::InitMaxHealth(float maxHealth)
{
	// 저장
	MaxHealth = maxHealth;
}

void UHealthBarWidget::SetCurrentHealth(float hp)
{
	if (HealthBar)
	{
		// 체력바도 업데이트
		HealthBar->SetPercent(FMath::Clamp(hp / MaxHealth, 0.0f, 1.0f));		
	}
}

void UHealthBarWidget::SetUserName(const FString& UserName) const
{
	if (UserNameTextBlock)
	{
		UserNameTextBlock->SetText(FText::FromString(UserName));
	}
}

void UHealthBarWidget::ApplyMyTeamMode()
{
	if (HealthBar)
	{
		HealthBar->SetFillColorAndOpacity(FLinearColor(0.0f, 0.5f, 1.0f, 1.0f));
	}

	if (UserNameTextBlock)
	{
		UserNameTextBlock->SetColorAndOpacity(FLinearColor(0.0f, 0.5f, 1.0f, 1.0f));
	}
}

void UHealthBarWidget::ApplyEnemyTeamMode()
{
	if (HealthBar)
	{
		HealthBar->SetFillColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
	}

	if (UserNameTextBlock)
	{
		UserNameTextBlock->SetColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
	}
}
