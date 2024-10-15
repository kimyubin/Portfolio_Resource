// Fill out your copyright notice in the Description page of Project Settings.

#include "EnochFightResultUI.h"
#include "Components/TextBlock.h"
#include "Enoch/EnochGameModeBase.h"


void UEnochFightResultUI::NativeConstruct()
{
	Super::NativeConstruct();	
	FText ResultComment = FText::FromString(TEXT("Error"));	
}
void UEnochFightResultUI::SetResult(GameResult InResult)
{
	FText ResultComment = FText::FromString(TEXT("Error"));
	switch (InResult)
	{
	case GameResult::None:
		break;
	case GameResult::UnderSimulated:
		break;
	case GameResult::Draw:
		ResultComment = FText::FromString(TEXT("Draw"));
		break;
	case GameResult::Win:
		ResultComment = FText::FromString(TEXT("WIN!"));
		break;
	case GameResult::Lose:
		ResultComment = FText::FromString(TEXT("Lose"));
		break;
	}
	
	ResultText->SetText(ResultComment);
	PlayAnimation(BlurAnim);
}

void UEnochFightResultUI::OnNextButtonDown()
{
	//일단 임시로 뷰포트에서 제거하는 함수만 놔뒀음.
	RemoveFromParent();
	
}
