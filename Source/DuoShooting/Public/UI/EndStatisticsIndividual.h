// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndStatisticsIndividual.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API UEndStatisticsIndividual : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* UserName;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Score;

public:
	void SetScore(int32 score);
	void SetUserName(FString name);
};
