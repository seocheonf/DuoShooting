// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PingUI.generated.h"

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API UPingUI : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Ping1;
	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Ping2;
	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Ping3;
	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Ping4;
	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Ping5;

public:
	void SetPingUI(float currentPing);
	
};
