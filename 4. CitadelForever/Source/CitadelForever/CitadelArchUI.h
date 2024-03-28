// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CitadelUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "CitadelArchUI.generated.h"

/**
 * 성채 건축 화면용 UI
 */
UCLASS()
class CITADELFOREVER_API UCitadelArchUI : public UCitadelUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	//virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable)
	class UCitadelPlayData* GetCitadelPlayData();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Select_00;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Select_01;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Select_02;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UWidgetSwitcher* ArcWidgetSwitcher;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UHorizontalBox* HorizonBox_Rampart;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UHorizontalBox* HorizonBox_Tower;
	
	UPROPERTY(BlueprintReadWrite)
	int ActiveWidgetIdx;
private:
	TWeakObjectPtr<class UCitadelPlayData> MyCitadelPlayData;
};
