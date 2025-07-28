// Fill out your copyright notice in the Description page of Project Settings.


#include "SteamInventory.h"

#include <steam/isteamuser.h>

#include <list>
#include <vector>

#include "KuTypes.h"
#include "UtlLog.h"


FOnlineLocalInventory::FOnlineLocalInventory()
{
	UT_LOG("TestOnlineInventory")
}

FSteamLocalInventory::FSteamLocalInventory()
	: m_hPlaytimeRequestResult(k_SteamInventoryResultInvalid)
	, m_hPromoRequestResult(k_SteamInventoryResultInvalid)
	, m_hLastFullUpdate(k_SteamInventoryResultInvalid)
	, m_hExchangeRequestResult(k_SteamInventoryResultInvalid)
	, LastDropInstanceID(k_SteamItemInstanceIDInvalid)
{
	// 이 게임이 아이템 정의 속성을 사용한다는 것을 나타냅니다("이름"을 검색합니다).
	// 게임에서 전체 항목 세트를 하드코딩하는 경우 이 호출을 건너뛸 수 있습니다.
	SteamInventory()->LoadItemDefinitions();
	
	// 귀하의 게임에서 제공하는(또는 향후 제공할 수 있는) 프로모션 항목이 있는 경우 해당 항목을 부여하는 호출입니다.
	// 프로모션 아이템은 다른 특정 게임을 소유하는 것과 같은 일부 외부 기준을 충족한 결과입니다.
	// 이러한 기준은 Steamworks 아이템 정의에 지정되어 있습니다.
	SteamInventory()->GrantPromoItems(&m_hPromoRequestResult);
	
	// SteamInventoryResultReady_t의 핸들과 비교하기 위해 결과 핸들을 수신하는 변수를 전달할 수 있지만,
	// 이 간단한 예는 여러 개의 진행 중인 API 호출을 추적하는 데 방해가 되지 않습니다.
	SteamInventory()->GetAllItems(nullptr); // 그러면 FullUpdate가 실행되고 ResultReady가 실행됩니다.
}

void FSteamLocalInventory::CheckForItemDrops()
{
	SteamInventory()->TriggerItemDrop( &m_hPlaytimeRequestResult, k_SpaceWarItem_TimedDropList );

}

void FSteamLocalInventory::OnSteamInventoryResult(SteamInventoryResultReady_t* pParam)
{
	// Ignore results that belong to some other SteamID - this normally won't happen, unless you start
	// calling SerializeResult/DeserializeResult, but it is better to be safe. Also ignore anything that
	// we just processed in OnSteamInventoryFullUpdate to avoid duplicate work.

	// 다른 SteamID에 속하는 결과를 무시합니다.
	// 일반적으로 SerializeResultDeserializeResult 호출을 시작하지 않는 한 이런 일은 발생하지 않지만, 안전을 유지하는 것이 더 좋습니다.
	// 또한 중복 작업을 피하기 위해 OnSteamInventoryFullUpdate에서 방금 처리한 내용은 모두 무시합니다.
	
	if (pParam->m_result == k_EResultOK
		&& pParam->m_handle != m_hLastFullUpdate
		&& SteamInventory()->CheckResultSteamID(pParam->m_handle, /*SpaceWarClient()->GetLocalSteamID()*/SteamUser()->GetSteamID()))
	{
		bool bGotResult = false;
		std::vector<SteamItemDetails_t> SteamItemList;
		uint32 count = 0;
		if (SteamInventory()->GetResultItems(pParam->m_handle, NULL, &count))
		{
			SteamItemList.resize(count);
			bGotResult = SteamInventory()->GetResultItems(pParam->m_handle, SteamItemList.data(), &count);
		}

		if (bGotResult)
		{
			// 이미 인벤토리에 있는 모든 항목에 대해 업데이트 또는 제거를 확인하세요.
			std::list<std::shared_ptr<FSteamItemData>>::iterator iter;
			for (iter = PlayerItems.begin(); iter != PlayerItems.end(); /*incr at end of loop*/)
			{
				bool bDestroy = false;
				for (size_t i = 0; i < SteamItemList.size(); ++i)
				{
					if ((*iter)->GetItemId() == SteamItemList[i].m_itemId)
					{
						// If flagged for removal by a partial update, remove it
						if (SteamItemList[i].m_unFlags & k_ESteamItemRemoved)
						{
							bDestroy = true;
						}
						else
						{
							(*iter)->SteamItemDetails = SteamItemList[i];
						}

						// 업데이트를 처리하면서 결과 벡터에서 요소를 제거합니다(빠른 스왑 앤 팝 제거).
						if (i < SteamItemList.size() - 1)
							SteamItemList[i] = SteamItemList.back();
						SteamItemList.pop_back();

						break;
					}
				}

				if (bDestroy)
				{
					// 현재 반복기에서 목록 요소를 삭제하고 진행합니다.
					iter = PlayerItems.erase(iter);
				}
				else
				{
					// 삭제하지 않고 반복자를 증가시킵니다.
					++iter;
				}
			}

			// Anything remaining in the result vector is a new item, unless flagged for removal by an operation result.
			// 작업 결과에 의해 제거 플래그가 지정되지 않는 한, 결과 벡터에 남아 있는 모든 항목은 새 항목입니다.
			for (size_t i = 0; i < SteamItemList.size(); ++i)
			{
				if ((SteamItemList[i].m_unFlags & k_ESteamItemRemoved) == false)
				{
					PlayerItems.emplace_back(std::make_shared<FSteamItemData>(SteamItemList[i]));
				}
			}
		}
	}

	// 보류 중인 핸들을 지웁니다.
	if (pParam->m_handle == m_hPlaytimeRequestResult)
		m_hPlaytimeRequestResult = -1;
	if (pParam->m_handle == m_hExchangeRequestResult)
		m_hExchangeRequestResult = -1;
	if (pParam->m_handle == m_hPromoRequestResult)
		m_hPromoRequestResult = -1;
	if (pParam->m_handle == m_hLastFullUpdate)
		m_hLastFullUpdate = -1;


	// 처리 후 결과에 매달리지 않습니다.
	SteamInventory()->DestroyResult(pParam->m_handle);
}

void FSteamLocalInventory::OnSteamInventoryFullUpdate(SteamInventoryFullUpdate_t* pParam)
{

	
	m_hLastFullUpdate = pParam->m_handle;
}