// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EndStatisticsIndividual.h"

#include "Components/TextBlock.h"

void UEndStatisticsIndividual::SetScore(int32 score)
{
	if (Score)
	{
		FText ScoreText = FText::Format(
			NSLOCTEXT("EndStatisticsIndividual", "ScoreText", "{0} points"), // 앞의 두 개는 일단 아무거나
			FText::AsNumber(score)
		);

		Score->SetText(ScoreText);
	}
}

void UEndStatisticsIndividual::SetUserName(FString name)
{
	if (UserName)
	{
		UserName->SetText(FText::FromString(name));
	}
}
