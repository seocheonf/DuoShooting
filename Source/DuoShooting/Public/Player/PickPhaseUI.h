// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SizeBox.h"
#include "Management/EnumContainer.h"
#include "PickPhaseUI.generated.h"

USTRUCT()
struct FSelectionHeroArea
{
	GENERATED_BODY()
	
	class UBorder* Border;
	class UButton* Button;
	bool bSelected = false;
	EHeroInfo HeroInfo;
	FString Name;
};

/**
 * 
 */
UCLASS()
class DUOSHOOTING_API UPickPhaseUI : public UUserWidget
{
	GENERATED_BODY()

	//==상속==//
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
	//==변수==
private:
	//주인
	class APickPhasePawn* Owner;

	//로컬 게임 스테이트
	class ATeamFightGameState* GameState;
	
	//포커스 스케일
	FVector2D FocusScale = FVector2D(1.25f, 1.25f);;

	//현재 선택된 영웅
	FSelectionHeroArea* CurrentSelectedHeroArea;
	
	//영웅 선택 영역
	//트레이서
	FSelectionHeroArea TracerArea;
	UPROPERTY(meta = (BindWidget))
	class UBorder* Border_Tracer;
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Tracer;
	//솜브라
	FSelectionHeroArea SombraArea;
	UPROPERTY(meta = (BindWidget))
	class UBorder* Border_Sombra;
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Sombra;

	//영웅 이름 노출
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_HeroName;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* HeroNameFade;

	//영웅 선택 완료 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_DecisionHero;

	//선택된 영웅 모델용 액터
	class ACharacter* CurrentModel;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true), Category=Model)
	TSubclassOf<class ACharacter> TracerModel;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true), Category=Model)
	TSubclassOf<class ACharacter> SombraModel;

	//남은 대기 시간 보더
	UPROPERTY(meta = (BindWidget))
	class UBorder* Border_RemainTime;
	//남은 대기 시간 글자
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_RemainTime;
	
	//==함수==
private:

	//각 버튼이 호버링 될 때 해야 할 일
	UFUNCTION()
	void OnHovered_Button_Tracer();
	UFUNCTION()
	void OnHovered_Button_Sombra();
	//각 버튼이 호버링 취소 될 때 해야 할 일
	UFUNCTION()
	void OnUnHovered_Button_Tracer();
	UFUNCTION()
	void OnUnHovered_Button_Sombra();
	//각 버튼이 클릭되었을 때 할 일
	UFUNCTION()
	void OnPressed_Button_Tracer();
	UFUNCTION()
	void OnPressed_Button_Sombra();

	
	//영웅 버튼이 호버링 될 때 해야할 일
	void OnHoveredHeroSelectButton(FSelectionHeroArea& characterArea);
	//영웅 버튼이 호버링 취소될 때 해야할 일
	void OnUnHoveredHeroSelectButton(FSelectionHeroArea& characterArea);
	//영웅 버튼이 클릭 되었을 때 해야할 일
	void OnPressedHeroSelectButton(FSelectionHeroArea& characterArea);
	
	//현재 선택된 영웅 지울 때 해야할 일
	void UnSetCurrentSelectedHeroArea();
	//현재 선택된 영웅 갱신할 때 해야할 일
	void SetCurrentSelectedHeroArea(FSelectionHeroArea& characterArea);

	//영웅 선택을 완료했을 때 해야할 일 (여기는 Clicked임)
	UFUNCTION()
	void OnClickedDecisionHeroButton();

	//영웅에 맞는 모델 찾기 함수
	TSubclassOf<class ACharacter> FindModel(EHeroInfo heroInfo);
	
};
