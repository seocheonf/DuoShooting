// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndStatisticsMain.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API UEndStatisticsMain : public UUserWidget
{
	GENERATED_BODY()

	UFUNCTION()
	void OnQuit_Clicked();
	
protected:
	UPROPERTY(meta=(BindWidget))
	class UButton* Quit;

public:
	virtual void NativeConstruct() override;
};
