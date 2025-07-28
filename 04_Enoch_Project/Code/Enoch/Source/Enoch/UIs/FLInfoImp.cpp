// Fill out your copyright notice in the Description page of Project Settings.


#include "FLInfoImp.h"

#include "Enoch/EnochGameInstance.h"
#include "../Common/EnochFreeLancerData.h"
#include "Enoch/EnochFight.h"
#include "../Common/EnochActorDataFactory.h"

void UFLInfoImp::NativeConstruct()
{
	SelectedSN = 0;
	SelectedID = FreeLancerTemplateID::None;
	Cast<UEnochGameInstance>(GetGameInstance())->FlInfo = this;
	Reset();
}

void UFLInfoImp::UpdateFLInfo()
{
	auto FreeLancerData = EnochActorDataFactory::instance->GetFreeLancerData(SelectedSN);
	//필드 용병 있으면 true;
	bool isFieldFreeLancer = (FreeLancerData!=nullptr);
	
	if(SelectedSN != 0 && SelectedID != FreeLancerTemplateID::None)
		Reset();
	// 선택 안된 상태	
	else if(SelectedSN==0 && SelectedID == FreeLancerTemplateID::None)
	{
		switcher->SetActiveWidgetIndex(0);
	}
	// 선택된 상태
	else
	{
		switcher->SetActiveWidgetIndex(1);

		//필드용병인 경우 SN기반으로 정보 불러옴
		if (isFieldFreeLancer)
		{
			// Unreal 들어가는 변수는 typedef 타입을 못쓴다!!
			SelectedID = (uint8_t)FreeLancerData->GetTID();
			SelectedLevel = FreeLancerData->GetLevel();		// 필드에서는 레벨이 0 ~ 4 이므로 그대로 쓴다
		}

		auto tmp = FreeLancerTemplate::GetFreeLancerTemplate(SelectedID);
		if( !tmp )
			return;
		auto &lvData = tmp->levelData[SelectedLevel];
		Name = WCHAR_TO_TCHAR(tmp->name.c_str());

		//이미지 세팅
		auto brush = Cast<UEnochGameInstance>(GetGameInstance())
			->MyPlayMenuUI->GetSlotImg(SelectedID);
		SlotImg->SetBrushFromMaterial(brush);
		
		// Star 세팅
		{
			// lv = 별 갯수, grade = 별 색깔
			std::wstring star = L"";
			// 필드에서는 레벨이 0 ~ 4 이므로 하나 더한다
			for( int i = 0; i < SelectedLevel + 1; i++ )
				star += L"★";
			FString fstrStar = star.c_str();
			FText starText = FText::FromString(fstrStar);
			txtStar->SetText(starText);

			switch(tmp->grade) // grade는 1~5다
			{
			case 1: txtStar->SetColorAndOpacity(FLinearColor::White); break;
			case 2: txtStar->SetColorAndOpacity(FLinearColor::Green); break;
			case 3: txtStar->SetColorAndOpacity(FLinearColor::Blue); break;
			case 4: txtStar->SetColorAndOpacity(FLinearColor::Red); break;
			case 5: txtStar->SetColorAndOpacity(FLinearColor::Yellow); break;
			}
			
		}

		// Alliance 세팅(임시)
		{
			std::wstring strAlly = L"";

			for (auto alliID : tmp->alliance)
			{
				auto alliTmp = AllianceTemplate::GetAllianceTemplate(alliID);
				if(!strAlly.empty())
					strAlly += L"\r\n";
				strAlly += alliTmp->name;
				if (isFieldFreeLancer)
				{
					uint8_t level = EnochActorDataFactory::instance->GetAllianceLevel(FreeLancerData->isEnemy, alliID);
					if (level > 0)
						strAlly += L"(" + to_wstring(level) + L")";
				}
			}
			FString fstrAlly = strAlly.c_str();
			FText ftxtAlly = FText::FromString(fstrAlly);
			TB_Alliance->SetText(ftxtAlly);
		}

		// HP, MP 세팅
		if (FreeLancerData)
		{
			floatHP = FreeLancerData->hpNow / FreeLancerData->hpMax;
			strHP = FString::Printf(TEXT("%d/%d %+.2g"), (int)FreeLancerData->hpNow, FreeLancerData->hpMax
				, FreeLancerData->hpRegeneration);
			

			if(FreeLancerData->mpMax != 0)
				floatMP = FreeLancerData->mpNow / FreeLancerData->mpMax;
			else
				floatMP = 0.0f;
			strMP = FString::Printf(TEXT("%d/%d %+.2g"), (int)FreeLancerData->mpNow, FreeLancerData->mpMax
				, FreeLancerData->mpRegeneration);
		}
		else
		{
			floatHP = 1.0f;
			strHP = FString::Printf(TEXT("%d/%d"), lvData.hp, lvData.hp);
			floatMP = 0.0f;
			strMP = FString::Printf(TEXT("0/%d"), lvData.mp);

		}

		if (FreeLancerData) {
			Range = FString::Printf(TEXT("%d"), FreeLancerData->attackRange);
			DmgBase = FString::Printf(TEXT("%d"), FreeLancerData->attackDamage);
			ASBase = FString::Printf(TEXT("%d"), FreeLancerData->attackSpeed);
			MSBase = FString::Printf(TEXT("%d"), tmp->moveSpeed);
			MSPercent = FString::Printf(TEXT("%d%%"), 100);
			Reduce = FString::Printf(TEXT("%d%%"), 0);
			ArmBase = FString::Printf(TEXT("%d"), FreeLancerData->defense);
			MRBase = FString::Printf(TEXT("%d"), FreeLancerData->magicRegist);

			float AtkPerSec = 1 / FreeLancerData->timeForOneAttack;
			DPS = FString::Printf(TEXT("%.2f"), AtkPerSec * FreeLancerData->attackDamage);
			APS = FString::Printf(TEXT("%.2f"), AtkPerSec);
			// physicalReduce, magicRegist 는 받는 데미지 비율 이므로, 1 - 값을 해야 저항력이 나옴
			PDef = FString::Printf(TEXT("%.2f%%"), (FreeLancerData->physicalReduce - 1.f) * 100);
			MDef = FString::Printf(TEXT("%.2f%%"), (FreeLancerData->magicRegist - 1.f) * 100);
		}
		else {
			Range = FString::Printf(TEXT("%d"), tmp->attackRange);
			DmgBase = FString::Printf(TEXT("%d"), lvData.atkDamage);
			ASBase = FString::Printf(TEXT("%d"), lvData.attackSpeed);
			MSBase = FString::Printf(TEXT("%d"), tmp->moveSpeed);
			MSPercent = FString::Printf(TEXT("%d%%"), 100);
			Reduce = FString::Printf(TEXT("%d%%"), 0);
			ArmBase = FString::Printf(TEXT("%d"), lvData.armor);
			MRBase = FString::Printf(TEXT("%d"), lvData.magicArmor);

			// DPS등은 임시로 계산
			const static float BaseAttackTime = 1.7f;
			float timeForOneAttack = BaseAttackTime / (1 + lvData.attackSpeed / 100);
			float AtkPerSec = 1 / timeForOneAttack;
			DPS = FString::Printf(TEXT("%.2f"), AtkPerSec * lvData.atkDamage);
			APS = FString::Printf(TEXT("%.2f"), AtkPerSec);
			float pdef = 0.1 * lvData.armor / (1.0 + 0.1 * abs(lvData.armor));
			PDef = FString::Printf(TEXT("%.2f%%"), pdef * 100);
			MDef = FString::Printf(TEXT("%.2f%%"), ((float)lvData.magicArmor) / 100.0);
		}

		/*
		// 임시로 추가/감소 데미지 표시
		static int add = 0;
		if (add)
		{
			add = 0;
			DmgAdd = FString::Printf(TEXT("+%d"), 5);
			ASAdd = FString::Printf(TEXT("+%d"), 5);
			ArmAdd = FString::Printf(TEXT("+%d"), 5);
			MRAdd = FString::Printf(TEXT("+%d"), 5);
		}
		else
		{
			add = 1;
			DmgAdd.Empty();
			ASAdd.Empty();
			ArmAdd.Empty();
			MRAdd.Empty();
		}
		*/
	}
}

void UFLInfoImp::SetFLSN(int32 SN)
{
	SelectedSN = SN;
	SelectedID = FreeLancerTemplateID::None;
	SelectedLevel = 0;
	UpdateFLInfo();
}

void UFLInfoImp::SetFLID(uint8 ID, uint8 level)
{
	SelectedSN = 0;
	SelectedID = ID;
	// UniformSlot 에서 온 레벨은 1~5 이므로 하나 빼준다.
	SelectedLevel = level - 1;
	UpdateFLInfo();
}

void UFLInfoImp::Reset()
{
	SelectedSN = 0;
	SelectedID = FreeLancerTemplateID::None;
	UpdateFLInfo();
}
