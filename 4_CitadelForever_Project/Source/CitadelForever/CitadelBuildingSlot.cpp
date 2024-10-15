// Fill out your copyright notice in the Description page of Project Settings.


#include "CitadelBuildingSlot.h"

#include "CitadelBuildingFactory.h"
#include "CitadelForeverGameInstance.h"
#include "CitadelGameUIsHandler.h"
#include "CitadelPlayData.h"
#include "CitadelPlayerController.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UCitadelBuildingSlot::NativeConstruct()
{
	Super::NativeConstruct();

	//버튼 투명화 및 함수 바인딩
	FButtonStyle StopStyle;
	FSlateBrush StopBrush;
	StopBrush.DrawAs = ESlateBrushDrawType::Box;
	StopBrush.TintColor = FSlateColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
	StopBrush.Margin = FMargin(0.f);
	StopStyle.Normal = StopBrush;
	StopStyle.Hovered = StopBrush;
	StopStyle.Pressed = StopBrush;
	StopStyle.Disabled = StopBrush;
	MySlotButton->SetStyle(StopStyle);
	
	MySlotButton->OnClicked.AddDynamic(this, &UCitadelBuildingSlot::SpawnPreviewBuilding);
	SetBuilding(MyBuildingType);
}

void UCitadelBuildingSlot::SpawnPreviewBuilding()
{
	GetGameInstance<UCitadelForeverGameInstance>()->GetCitadelGameUIsHandler()->DeselectBuilding();
	
	const auto CitadelPC = Cast<ACitadelPlayerController>(GetOwningPlayer());
	
	//델리케이트에 바인딩된 함수에 슬롯 정보 전송
	DelegateTrackPreviewBuilding.BindUFunction(CitadelPC,FName("SpawnAndTrackPreviewBuilding"));
	if(DelegateTrackPreviewBuilding.IsBound() == true)
		DelegateTrackPreviewBuilding.Execute(MyBuildingType);
}

void UCitadelBuildingSlot::SetBuilding(EBuildingType InBuildingType)
{
	MyBuildingType = InBuildingType;
	MySlotImg->SetBrushFromTexture(GetGameInstance<UCitadelForeverGameInstance>()->GetCitadelPlayData()->GetBuildingUIImage(MyBuildingType));	
}


ACitadelBuildingFactory* UCitadelBuildingSlot::GetBuildingFactory()
{	
	if(!MyCitadelBuildingFactory.IsValid())	
		MyCitadelBuildingFactory = GetGameInstance<UCitadelForeverGameInstance>()->GetCitadelBuildingFactory();
	
	return MyCitadelBuildingFactory.Get();
}
