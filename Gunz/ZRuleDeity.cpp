#include "stdafx.h"
#include "ZRuleDeity.h"

ZRuleDeity::ZRuleDeity(ZMatch* pMatch) : ZRule(pMatch), m_uidDeity(0,0), m_fBonusDamage(1.5f)
{

}

ZRuleDeity::~ZRuleDeity()
{


}

bool ZRuleDeity::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{
	case MC_MATCH_ASSIGN_DEITY:
		{
			MUID uidDeity;
			pCommand->GetParameter(&uidDeity,		0, MPT_UID);

			AssignDeity(uidDeity);
		}
		break;
	case MC_MATCH_GAME_DEAD:
		{
			MUID uidAttacker, uidVictim;
			unsigned long int nAttackerArg, nVictimArg;

			pCommand->GetParameter(&uidAttacker, 0, MPT_UID);
			pCommand->GetParameter(&nAttackerArg, 1, MPT_UINT);
			pCommand->GetParameter(&uidVictim, 2, MPT_UID);
			pCommand->GetParameter(&nVictimArg, 3, MPT_UINT);


			bool bSuicide = false;
			if (uidAttacker == uidVictim) bSuicide = true;

			if ((uidAttacker != MUID(0,0)) && (uidAttacker == m_uidDeity))
			{
				if (!bSuicide)
				{
					ZCharacter* pAttacker = ZGetGame()->m_CharacterManager.Find(uidAttacker);
					OnBonus(pAttacker);
				}
			}
		}
		break;
	}

	return false;
}

void ZRuleDeity::OnResponseRuleInfo(MTD_RuleInfo* pInfo)
{
	MTD_RuleInfo_Berserker* pDeityRule = (MTD_RuleInfo_Berserker*)pInfo;
	AssignDeity(pDeityRule->uidBerserker);
}


void ZRuleDeity::AssignDeity(MUID& uidDeity)
{
	if (!ZGetGame()) return;

	for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
		itor != ZGetGame()->m_CharacterManager.end(); ++itor)
	{
		ZCharacter* pCharacter = (*itor).second;
		pCharacter->SetTagger(false);
	}

	ZCharacter* pDeitychar = ZGetGame()->m_CharacterManager.Find(uidDeity);
	if (pDeitychar)
	{
		ZGetEffectManager()->AddBerserkerIcon(pDeitychar);
		pDeitychar->SetTagger(true);
		
		if (!pDeitychar->IsDie())
		{
			float fMaxHP = pDeitychar->GetProperty()->fMaxHP.Ref();
			float fMaxAP = pDeitychar->GetProperty()->fMaxAP.Ref();
			pDeitychar->SetHP(fMaxHP);
			pDeitychar->SetAP(fMaxAP);


			if (uidDeity == ZGetMyUID())
				ZGetGameInterface()->PlayVoiceSound( VOICE_GOT_BERSERKER, 1600);
			else
				ZGetGameInterface()->PlayVoiceSound( VOICE_BERSERKER_DOWN, 1200);
		}

	}

	m_uidDeity = uidDeity;
	m_fElapsedHealthUpdateTime = 0.0f;
}

void ZRuleDeity::OnUpdate(float fDelta)
{
	m_fElapsedHealthUpdateTime += fDelta;
}

void ZRuleDeity::OnBonus(ZCharacter* pDeity)
{
	if (pDeity)
	{
		if (pDeity->IsDie()) 
		{
			CHECK_RETURN_CALLSTACK(OnBonus);
			return;
		}

		if (m_fBonusDamage < 8.0f)
			m_fBonusDamage += 0.1f;

		float fBonusAP = 100.0f;
		float fBonusHP = 100;

		float fMaxHP = pDeity->GetProperty()->fMaxHP.Ref();

		if ((fMaxHP - pDeity->GetHP()) < 100)
		{
			fBonusHP = fMaxHP - pDeity->GetHP();
			fBonusAP = 100 - fBonusHP;
		}

		pDeity->SetHP(pDeity->GetHP() + fBonusHP);
		pDeity->SetAP(pDeity->GetAP() + fBonusAP);
	}
	CHECK_RETURN_CALLSTACK(OnBonus);
}