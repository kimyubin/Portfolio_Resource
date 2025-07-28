// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CitadelGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class CITADELFOREVER_API ACitadelGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ACitadelGameModeBase();
protected:
	virtual void BeginPlay() override;
	virtual void SubBeginPlay();

	UFUNCTION(BlueprintCallable, Category = "UMG Game")
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);
	
public:

	UFUNCTION()
	FVector GetDefaultPawnSpawnLocation() const {return DefaultPawnSpawnLocation ;}
	UFUNCTION()
	FRotator GetDefaultPawnSpawnRotation() const {return DefaultPawnSpawnRotation ;}
	
	/** 엔진에서 사용할 위젯 클래스를 가져옴 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	TSubclassOf<UUserWidget> StartWidgetClass;
	
	/**
	 * 레벨 시작시 사용될 위젯UI
	 * 월드 별 초기 UI를 지정함
	 */
	UPROPERTY()
	UUserWidget* CurrentWidget;

	/** 디폴트 폰 대신 사용할 기본 폰 클래스. 스폰 위치 등을 제어하기 위해 별개로 구성*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defined Class")
	TSubclassOf<class ACitadelDefaultPawn> CitadelControlPawnClass;

	/** 빌딩 스폰 팩토리 클래스*/	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defined Class")
	TSubclassOf<class ACitadelBuildingFactory> CitadelBuildingFactoryClass;

	/** 디폴트 폰 스폰 위치 지정.*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Default Pawn")
	FVector DefaultPawnSpawnLocation;

	/** 디폴트 폰 스폰 각도 지정. X:Roll, Y:Pitch, Z:Yaw*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Default Pawn")
	FRotator DefaultPawnSpawnRotation;
	
};
