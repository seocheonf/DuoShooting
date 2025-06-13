// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PickPhasePawn.h"
#include "Player/PickPhaseUI.h"

#include "Management/TeamFightGameMode.h"
#include "Management/TeamFightGameState.h"


// Sets default values
APickPhasePawn::APickPhasePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FClassFinder<UPickPhaseUI> ui(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/DuoShooting/UIs/WBP_PickPhaseUI.WBP_PickPhaseUI_C'"));
	if (ui.Succeeded())
	{
		OriginPickPhaseUI = ui.Class;
	}
}

// Called when the game starts or when spawned
void APickPhasePawn::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		PickPhaseUI = CreateWidget<UPickPhaseUI>(GetWorld(), OriginPickPhaseUI);
		PickPhaseUI->AddToViewport();
		APlayerController* playerController = Cast<APlayerController>(Controller);
		playerController->SetInputMode(FInputModeUIOnly());
		playerController->bShowMouseCursor = true;
	}


	/*
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [&]()
	{
		if (Cast<APlayerController>(Controller) != nullptr && IsLocallyControlled())
		{
			int a = FMath::RandRange(0, 1);
			if (a == 0)
				ServerRPC_SetPlayerHero(EHeroInfo::Sombra);
			else if (a == 1)
				ServerRPC_SetPlayerHero(EHeroInfo::Tracer);
		}
	}, 1, false);
	FTimerHandle timerHandle;
	GetWorldTimerManager().SetTimer(timerHandle, [&]()
	{
		if (Cast<APlayerController>(Controller) != nullptr && IsLocallyControlled())
			ServerRPC_RespawnPlayer();
	}, 2, false);
	*/
}

void APickPhasePawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//if (EndPlayReason == EEndPlayReason::Type::)
	Super::EndPlay(EndPlayReason);
	
	if (nullptr != PickPhaseUI)
	{
		PickPhaseUI->RemoveFromParent();
		//이미 파괴된 이후 마지막이니, 일반 플레이어 컨트롤러로서 정보를 가져와야 함.
		APlayerController* playerController = GetWorld()->GetFirstPlayerController();
		playerController->SetInputMode(FInputModeGameOnly());
		playerController->bShowMouseCursor = false;
	}
	
}

// Called every frame
void APickPhasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APickPhasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APickPhasePawn::ServerRPC_SetPlayerHero_Implementation(EHeroInfo playerHero)
{
	//현재 월드의 게임 모드를 가져온다.
	ATeamFightGameMode* gameMode = Cast<ATeamFightGameMode>(GetWorld()->GetAuthGameMode());

	//만약 게임 모드가 없다면, return한다.
	//이 함수가 server rpc implementation이라 서버에서 돌아감이 보장되므로 nullptr이라면
	//서버에 게임 모드가 없는 비 정상적 상황인 것이기에 에러 메시지를 띄운다.
	if (nullptr == gameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode does not exist in Server!!!!!!"))
		return;
	}

	APlayerController* playerController = Cast<APlayerController>(Controller);
	//받은 정보를 바탕으로 플레이어의 영웅 정보를 갱신시킨다.
	if (nullptr != playerController)
		gameMode->SetPlayerHero(playerController, playerHero);
}

void APickPhasePawn::ServerRPC_RespawnPlayer_Implementation()
{
	//현재 월드의 게임 모드를 가져온다.
	ATeamFightGameMode* gameMode = Cast<ATeamFightGameMode>(GetWorld()->GetAuthGameMode());

	//만약 게임 모드가 없다면, return한다.
	//이 함수가 server rpc implementation이라 서버에서 돌아감이 보장되므로 nullptr이라면
	//서버에 게임 모드가 없는 비 정상적 상황인 것이기에 에러 메시지를 띄운다.
	if (nullptr == gameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode does not exist in Server!!!!!!"))
		return;
	}

	APlayerController* playerController = Cast<APlayerController>(Controller);
	//받은 정보를 바탕으로 플레이어의 영웅 정보를 갱신시킨다.
	if (nullptr != playerController)
		gameMode->RespawnPlayer(playerController);
}
