// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PickPhaseUI.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Management/NetworkGameInstance.h"
#include "Management/TeamFightGameMode.h"
#include "Management/TeamFightGameState.h"
#include "Management/TeamFightPlayerController.h"
#include "Player/HeroBase.h"
#include "Player/PickPhasePawn.h"

void UPickPhaseUI::NativeConstruct()
{
	Super::NativeConstruct();

	Owner = Cast<APickPhasePawn>(GetWorld()->GetFirstPlayerController()->GetPawn());

	//영웅 관련 정보 등록
	TracerArea = FSelectionHeroArea(Border_Tracer, Button_Tracer, false, EHeroInfo::Tracer, FString("Tracer"));
	SombraArea = FSelectionHeroArea(Border_Sombra, Button_Sombra, false, EHeroInfo::Sombra, FString("Sombra"));

	//영웅 버튼 호버링 시 할 일
	TracerArea.Button->OnHovered.AddDynamic(this, &UPickPhaseUI::OnHovered_Button_Tracer);
	SombraArea.Button->OnHovered.AddDynamic(this, &UPickPhaseUI::OnHovered_Button_Sombra);

	//영웅 버튼 호버링 아웃
	TracerArea.Button->OnUnhovered.AddDynamic(this, &UPickPhaseUI::OnUnHovered_Button_Tracer);
	SombraArea.Button->OnUnhovered.AddDynamic(this, &UPickPhaseUI::OnUnHovered_Button_Sombra);

	//영웅 버튼 클릭 시점에 할 일
	TracerArea.Button->OnPressed.AddDynamic(this, &UPickPhaseUI::OnPressed_Button_Tracer);
	SombraArea.Button->OnPressed.AddDynamic(this, &UPickPhaseUI::OnPressed_Button_Sombra);

	//영웅 선택 완료 버튼 누르면 할 일
	Button_DecisionHero->OnClicked.AddDynamic(this, &UPickPhaseUI::OnClickedDecisionHeroButton);
	
	//캐릭터 하나 선택해 두기.
	SetCurrentSelectedHeroArea(TracerArea);

	//게임 스테이트 받아오기
	GameState = GetWorld()->GetGameState<ATeamFightGameState>();
}

void UPickPhaseUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (GameState->GetIsWaiting())
	{
		Border_RemainTime->SetVisibility(ESlateVisibility::Visible);
		Text_RemainTime->SetText(FText::FromString(FString::Printf(TEXT("게임 시작 %d초 전"), static_cast<int>(GameState->GetCurrnetRemainWaitingTime()))));
	}
	if (GameState->GetIsStart())
	{
		Button_DecisionHero->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPickPhaseUI::NativeDestruct()
{
	Super::NativeDestruct();
	
}

void UPickPhaseUI::OnHovered_Button_Tracer()
{
	OnHoveredHeroSelectButton(TracerArea);
}

void UPickPhaseUI::OnHovered_Button_Sombra()
{
	OnHoveredHeroSelectButton(SombraArea);
}

void UPickPhaseUI::OnUnHovered_Button_Tracer()
{
	OnUnHoveredHeroSelectButton(TracerArea);
}

void UPickPhaseUI::OnUnHovered_Button_Sombra()
{
	OnUnHoveredHeroSelectButton(SombraArea);
}

void UPickPhaseUI::OnPressed_Button_Tracer()
{
	OnPressedHeroSelectButton(TracerArea);
}

void UPickPhaseUI::OnPressed_Button_Sombra()
{
	OnPressedHeroSelectButton(SombraArea);
}

void UPickPhaseUI::OnHoveredHeroSelectButton(FSelectionHeroArea& characterArea)
{
	characterArea.Border->SetRenderScale(FocusScale);
}

void UPickPhaseUI::OnUnHoveredHeroSelectButton(FSelectionHeroArea& characterArea)
{
	//선택되어 있다면 하지 말 것.
	if (!characterArea.bSelected)
		characterArea.Border->SetRenderScale(FVector2D(1.f, 1.f));
}

void UPickPhaseUI::OnPressedHeroSelectButton(FSelectionHeroArea& characterArea)
{
	if (CurrentSelectedHeroArea == &characterArea)
		return;
	
	//기존 선택 상태 제거
	UnSetCurrentSelectedHeroArea();
	//새로운 선택 상태 설정
	SetCurrentSelectedHeroArea(characterArea);
}

void UPickPhaseUI::UnSetCurrentSelectedHeroArea()
{
	if (nullptr == CurrentSelectedHeroArea)
		return;
	//기존 선택된 캐릭터의 상태를 끄고, 원상복구
	CurrentSelectedHeroArea->bSelected = false;
	CurrentSelectedHeroArea->Border->SetRenderScale(FVector2D(1.f, 1.f));
	CurrentSelectedHeroArea->Border->SetBrushColor(FColor::White);
	//기존 선택된 캐릭터 이름 제거
	Text_HeroName->SetText(FText::GetEmpty());

	if (nullptr != CurrentModel)
		CurrentModel->Destroy();
}

void UPickPhaseUI::SetCurrentSelectedHeroArea(FSelectionHeroArea& characterArea)
{
	//선택된 캐릭터 교체
	CurrentSelectedHeroArea = &characterArea;
	//새롭게 선택된 캐릭터의 상태를 적절히 수정.
	CurrentSelectedHeroArea->bSelected = true;
	CurrentSelectedHeroArea->Border->SetRenderScale(FocusScale);
	CurrentSelectedHeroArea->Border->SetBrushColor(FColor::Orange);
	//새롭게 선택된 캐릭터의 이름 노출
	Text_HeroName->SetText(FText::FromString(CurrentSelectedHeroArea->Name));
	PlayAnimation(HeroNameFade);

	TSubclassOf<ACharacter> model = FindModel(CurrentSelectedHeroArea->HeroInfo);
	//더미 캐릭터 위치 앞에 생성하도록 임시 처리
	if (nullptr != model)
		CurrentModel = GetWorld()->SpawnActor<ACharacter>(model, Owner->GetActorLocation() + Owner->GetActorForwardVector() * 100.f, FRotator(0, -90, 0));

	//선택 즉시 현재 원하는 캐릭터 상태를 변경해 달라고 서버에 요청한다.
	Owner->ServerRPC_SetPlayerHero(CurrentSelectedHeroArea->HeroInfo);
}

void UPickPhaseUI::OnClickedDecisionHeroButton()
{
	//버튼들 비활성화
	SetInputActionBlocking(true);
	CurrentModel->Destroy();
	
	Owner->ServerRPC_SetPlayerHero(CurrentSelectedHeroArea->HeroInfo);
	Owner->ServerRPC_RespawnPlayer();

	if (ATeamFightPlayerController* pc = Cast<ATeamFightPlayerController>(Owner->GetController()))
	{
		pc->ServerRPC_RequestGameEnterNotice();
	}
}

void UPickPhaseUI::DestroyModel()
{
	if (CurrentModel != nullptr)
	{
		CurrentModel->Destroy();
	}
}


TSubclassOf<ACharacter> UPickPhaseUI::FindModel(EHeroInfo heroInfo)
{
	switch (heroInfo)
	{
		case EHeroInfo::Tracer:
			return TracerModel;
		case EHeroInfo::Sombra:
			return SombraModel;
		default:
			return nullptr;
	}
}




