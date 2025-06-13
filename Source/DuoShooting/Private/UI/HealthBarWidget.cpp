// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HealthBarWidget.h"
#include "Components/ProgressBar.h"


void UHealthBarWidget::InitMaxHealth(float hp)
{
	// 저장
	MaxHealth = hp;
}

void UHealthBarWidget::SetCurrentHealth(float hp)
{
	// 체력바도 업데이트
	HealthBar->SetPercent(FMath::Clamp(hp / MaxHealth, 0.0f, 1.0f));
}

void UHealthBarWidget::ApplyMyTeamMode()
{
	HealthBar->SetFillColorAndOpacity(FColor(0.0f, 0.5f, 1.0f, 1.0f));
}

void UHealthBarWidget::ApplyEnemyTeamMode()
{
	HealthBar->SetFillColorAndOpacity(FColor(1.0f, 0.0f, 0.0f, 1.0f));
}
