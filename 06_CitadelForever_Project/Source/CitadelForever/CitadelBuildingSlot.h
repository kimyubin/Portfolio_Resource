// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CitadelForeverTypes.h"
#include "CitadelUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "CitadelBuildingSlot.generated.h"

//빌딩 생성 버튼 클릭 시, 컨트롤러로 델리케이트 바인팅 
DECLARE_DELEGATE_OneParam( FDele_TrackPreviewBuilding, EBuildingType )


/**
 * 
 */
UCLASS()
class CITADELFOREVER_API UCitadelBuildingSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	/** 임시 빌딩 스폰 및 바인딩된 함수 호출 */
	UFUNCTION(BlueprintCallable)
	void SpawnPreviewBuilding();
	
	/** 빌딩 타입 결정 및 UI 이미지 변경*/
	UFUNCTION(BlueprintCallable)
	void SetBuilding(EBuildingType InBuildingType);
	
	UFUNCTION(BlueprintCallable)
	class ACitadelBuildingFactory* GetBuildingFactory();

protected:
	/** 슬롯 입력 흡수용 버튼. 버블되어 클릭 이벤트가 UI 뒤로 넘어가지 않게 함*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Buildings")	
	class UButton* MySlotButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Buildings")
	class UImage* MySlotImg;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Buildings")	
	EBuildingType MyBuildingType;

	TWeakObjectPtr<class ACitadelBuildingFactory> MyCitadelBuildingFactory;

public:
	FDele_TrackPreviewBuilding DelegateTrackPreviewBuilding;
};
