// Fill out your copyright notice in the Description page of Project Settings.


#include "FieldDragDetector.h"
#if defined(__WIN32__) || defined(__WIN64__)
#include <d3d10sdklayers.h>
#endif

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Enoch/EnochDragDropOP.h"
#include "Enoch/EnochMouseController.h"
#include "Kismet/GameplayStatics.h"

void UFieldDragDetector::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	auto EController = Cast<AEnochMouseController>(UGameplayStatics::GetPlayerController(this,0));
	UEnochGameInstance* EGameInstance = Cast<UEnochGameInstance>(GetGameInstance());

	MyCurrentCellLocation = EController->GetCurrentCellUnderCursor();
	MyFreeLancerTID = EController->GetCurrentFLUnderCursor().FLTemplateID;
	
	//용병없으면 금지
	if(MyFreeLancerTID==FreeLancerTemplateID::None)
		return;
	//적 진영에 드래그 금지.
	if(MyCurrentCellLocation.Y>=5)
		return;
	
	UEnochDragDropOP* DragOp = Cast<UEnochDragDropOP>(UWidgetBlueprintLibrary::CreateDragDropOperation(UEnochDragDropOP::StaticClass()));
	//드래그용 용병 이미지 업데이트	
	EGameInstance->MyPlayMenuUI->DragImage->SetDragImg(MyFreeLancerTID);
	
	//이미지 설정
	DragOp->DefaultDragVisual=Cast<UEnochGameInstance>(GetGameInstance())->MyPlayMenuUI->DragImage;

	if(DragOwnerFreeLancer!=nullptr)
	{
		//선택용병 업데이트
		EGameInstance->LiftFreeLancerInField(MyCurrentCellLocation,DragOwnerFreeLancer);
		//드래그할 때 이미지 숨김.
		DragOwnerFreeLancer->SetImageVisualOnOff(false);
	}		
	
	//스폰용 함수 바인딩. 함수 내부에 이동 로직도 포함함.
	DragOp->FDeleDragDropSpawnFL.BindUFunction(EController,FName("DragDropSpawnFL"));
	FVector2D NewPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	this->SetPositionInViewport(NewPosition, false);
	OutOperation = DragOp;
}

FReply UFieldDragDetector::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply Reply;
	Reply.NativeReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	UEnochDragDropOP* DragOp = Cast<UEnochDragDropOP>(UWidgetBlueprintLibrary::CreateDragDropOperation(UEnochDragDropOP::StaticClass()));	
	

	
	Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);	
	return Reply.NativeReply;
}
