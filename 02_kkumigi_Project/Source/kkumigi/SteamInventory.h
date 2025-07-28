// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <list>
#include <memory>
#include <string>
#include <steam/isteaminventory.h>
#include <steam/steam_api_common.h>

#include "CoreMinimal.h"
#include "KuObject.h"
#include "UObject/NoExportTypes.h"
#include "SteamInventory.generated.h"

class FSteamItemData;
/**
 * 
 */
UCLASS()
class KKUMIGI_API USteamInventory : public UKuObject
{
	GENERATED_BODY()
	
};

class FOnlineInventory
{
public:
	
};


/**
 * 온라인 서비스 대신 로컬에서 구현된 인벤토리입니다.
 * 테스트 환경에서 스팀 인벤토리를 대체합니다.
 */
#if WITH_EDITOR
class FOnlineLocalInventory : public FOnlineInventory
{
public:
	FOnlineLocalInventory();
};
#endif // WITH_EDITOR

class FSteamLocalInventory : public FOnlineInventory
{
public:
	FSteamLocalInventory();

	void CheckForItemDrops();
	
	STEAM_CALLBACK(FSteamLocalInventory, OnSteamInventoryResult, SteamInventoryResultReady_t);
	STEAM_CALLBACK(FSteamLocalInventory, OnSteamInventoryFullUpdate, SteamInventoryFullUpdate_t);

	
	std::list<std::shared_ptr<FSteamItemData>> PlayerItems;

	SteamInventoryResult_t m_hPlaytimeRequestResult;
	SteamInventoryResult_t m_hPromoRequestResult;
	SteamInventoryResult_t m_hLastFullUpdate;
	SteamInventoryResult_t m_hExchangeRequestResult;

	
	SteamItemInstanceID_t LastDropInstanceID;
};


class FSteamItemData
{
public:
	FSteamItemData() = default;
	explicit FSteamItemData(const SteamItemDetails_t& M_Details): SteamItemDetails(M_Details) {}

	/** 
	 * 각 개별 아이템 인스턴스가 갖는 전역적으로 고유한 ItemInstanceID를 반환합니다.
	 */
	SteamItemInstanceID_t GetItemId() const { return SteamItemDetails.m_itemId; }

	/**
	 * 아이템 유형을 식별하는 32비트 "아이템 정의 번호"를 반환합니다. 1~999999999(10억(1x10^9)미만)
	 */
	SteamItemDef_t GetDefinitionNumber() const { return SteamItemDetails.m_iDefinition; }

	/** 아이템 수량 */
	uint16 GetQuantity() const { return SteamItemDetails.m_unQuantity; }
	
	std::string GetLocalizedName() const;
	std::string GetLocalizedDescription() const;
	std::string GetIconURL() const;

private:
	friend class FSteamLocalInventory;
	SteamItemDetails_t SteamItemDetails;
};


class FInvenTest
{
public:
	FInvenTest();

	struct CCallbackInternal_OnSteamInventoryResult : private CCallbackImpl<sizeof(SteamInventoryResultReady_t)>
	{
		CCallbackInternal_OnSteamInventoryResult()
		{
			SteamAPI_RegisterCallback(this, SteamInventoryResultReady_t::k_iCallback);
		}

		CCallbackInternal_OnSteamInventoryResult(const CCallbackInternal_OnSteamInventoryResult&)
		{
			SteamAPI_RegisterCallback(this, SteamInventoryResultReady_t::k_iCallback);
		}

		CCallbackInternal_OnSteamInventoryResult& operator=(const CCallbackInternal_OnSteamInventoryResult&)
		{
			return *this;
		}

	private:
		virtual void Run(void* pvParam)
		{
			FInvenTest* pOuter = reinterpret_cast<FInvenTest*>(reinterpret_cast<char*>(this) - ((::size_t)&reinterpret_cast<char const volatile&>((((FInvenTest*)0)->
				m_steamcallback_OnSteamInventoryResult))));
			pOuter->OnSteamInventoryResult(reinterpret_cast<SteamInventoryResultReady_t*>(pvParam));
		}
	} m_steamcallback_OnSteamInventoryResult;

	
	void OnSteamInventoryResult(SteamInventoryResultReady_t* pParam);
	STEAM_CALLBACK(FInvenTest, OnSteamInventoryFullUpdate, SteamInventoryFullUpdate_t);

	std::list<std::shared_ptr<FSteamItemData>> PlayerItems;

	SteamInventoryResult_t PlaytimeRequestHandle;
	SteamInventoryResult_t PromoRequestHandle;
	SteamInventoryResult_t LastFullUpdateHandle;
	SteamInventoryResult_t ExchangeRequestHandle;

	SteamItemInstanceID_t LastDropInstanceID;
};

