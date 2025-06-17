// Fill out your copyright notice in the Description page of Project Settings.


#include "Management/EndStatisticsPlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "UI/EndStatisticsMain.h"

AEndStatisticsPlayerController::AEndStatisticsPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	{
		ConstructorHelpers::FClassFinder<UEndStatisticsMain> tempWidget(
			TEXT(
				"/Script/UMGEditor.WidgetBlueprint'/Game/DuoShooting/UIs/WBP_EndStatisticsMain.WBP_EndStatisticsMain_C'"));
		if (tempWidget.Succeeded()) { EndStatisticsMainFactory = tempWidget.Class; }
	}
}

void AEndStatisticsPlayerController::FindSharedCamera()
{
	// 월드에 있는 카메라를 모두 찾아오기
	TArray<AActor*> foundCameras;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), foundCameras);

	// Shared Camera라고 되어있는것 찾아서 뷰 타겟 바꾸기
	for (AActor* Actor : foundCameras)
	{
		if (ACameraActor* cameraActor = Cast<ACameraActor>(Actor))
		{
			UCameraComponent* cameraComponent = Cast<UCameraComponent>(
				cameraActor->GetComponentByClass(UCameraComponent::StaticClass()));

			if (cameraComponent && cameraComponent->ComponentHasTag(FName("SharedCamera")))
			{
				SetViewTarget(cameraActor);
			}
		}
	}
}

void AEndStatisticsPlayerController::CreateMainWidget()
{
	if (EndStatisticsMainFactory)
	{
		EndStatisticsMainWidget = CreateWidget<UEndStatisticsMain>(GetWorld(), EndStatisticsMainFactory);

		if (EndStatisticsMainWidget != nullptr)
		{
			EndStatisticsMainWidget->AddToViewport();
		}
	}
}

void AEndStatisticsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FindSharedCamera();
	CreateMainWidget();
}
