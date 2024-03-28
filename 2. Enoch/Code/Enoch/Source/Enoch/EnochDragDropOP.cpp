// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochDragDropOP.h"

void UEnochDragDropOP::DragCancelled_Implementation(const FPointerEvent& PointerEvent)
{
	Super::DragCancelled_Implementation(PointerEvent);
	
	//델리케이트에 바인딩된 함수에 화면에서 현재 좌표 전송.
	if(FDeleDragDropSpawnFL.IsBound() == true)
		FDeleDragDropSpawnFL.Execute(PointerEvent.GetScreenSpacePosition());
}
