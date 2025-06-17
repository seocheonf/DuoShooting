// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EndStatisticsMain.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UEndStatisticsMain::NativeConstruct()
{
	Super::NativeConstruct();

	Quit->OnClicked.AddDynamic(this, &UEndStatisticsMain::OnQuit_Clicked);
}

void UEndStatisticsMain::OnQuit_Clicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}
