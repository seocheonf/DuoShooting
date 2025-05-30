// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShootingMainWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UShootingMainWidget::InitMaxHealth(float hp)
{
	// 저장
	MaxHealth = hp;

	// int로 변환해서 UI로 보여주기
	MaxHealthText->SetText(FText::AsNumber(FMath::RoundToInt(hp)));
}

void UShootingMainWidget::InitMaxBullet(int32 bullets)
{
	MaxBulletText->SetText(FText::AsNumber(bullets));
}

void UShootingMainWidget::SetCurrentHealth(float hp)
{
	// int로 올림해서 UI로 보여주기
	CurrentHealthText->SetText(FText::AsNumber(FMath::CeilToInt(hp)));

	// 체력바도 업데이트
	HealthBar->SetPercent(FMath::Clamp(hp / MaxHealth, 0.0f, 1.0f));
}

void UShootingMainWidget::SetCurrentBullet(int32 bullets)
{
	CurrentBullet = bullets;
}
