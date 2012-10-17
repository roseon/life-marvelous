#include "stdafx.h"
#include "MMatchRuleDeity.h"
#include "MMatchTransDataType.h"
#include "MBlobArray.h"
#include "MMatchServer.h"

//////////////////////////////////////////////////////////////////////////////////
// MMatchRuleDeity ///////////////////////////////////////////////////////////
MMatchRuleDeity::MMatchRuleDeity(MMatchStage* pStage) : MMatchRuleSoloDeath(pStage), m_uidDeity(0,0)
{

}

bool MMatchRuleDeity::OnCheckRoundFinish()
{
	return MMatchRuleSoloDeath::OnCheckRoundFinish();
}

void MMatchRuleDeity::OnRoundBegin()
{
	m_uidDeity = MUID(0,0);
}

void* MMatchRuleDeity::CreateRuleInfoBlob()
{
	void* pRuleInfoArray = MMakeBlobArray(sizeof(MTD_RuleInfo_Deity), 1);
	MTD_RuleInfo_Deity* pRuleItem = (MTD_RuleInfo_Deity*)MGetBlobArrayElement(pRuleInfoArray, 0);
	memset(pRuleItem, 0, sizeof(MTD_RuleInfo_Deity));
	
	pRuleItem->nRuleType = MMATCH_GAMETYPE_DEITY;
	pRuleItem->uidDeity = m_uidDeity;

	return pRuleInfoArray;
}

void MMatchRuleDeity::RouteAssignDeity()
{	MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_ASSIGN_DEITY, MUID(0, 0));
	pNew->AddParameter(new MCmdParamUID(m_uidDeity));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);
}


MUID MMatchRuleDeity::RecommendDeity()
{
	MMatchStage* pStage = GetStage();
	if (pStage == NULL) return MUID(0,0);

	int nCount = 0;
	for(MUIDRefCache::iterator itor=pStage->GetObjBegin(); itor!=pStage->GetObjEnd(); itor++) {
		MMatchObject* pObj = (MMatchObject*)(*itor).second;
		if (pObj->GetEnterBattle() == false) continue;
		if (pObj->CheckAlive())
		{
			return pObj->GetUID();
		}
	}
	return MUID(0,0);

}


void MMatchRuleDeity::OnEnterBattle(MUID& uidChar)
{
}

void MMatchRuleDeity::OnLeaveBattle(MUID& uidChar)
{
	if (uidChar == m_uidDeity)
	{
		m_uidDeity = MUID(0,0);
		RouteAssignDeity();
	}
}

void MMatchRuleDeity::OnGameKill(const MUID& uidAttacker, const MUID& uidVictim)
{
	if ((m_uidDeity == uidVictim) || (m_uidDeity == MUID(0,0)))
	{
		bool bAttackerCanBeDeity = false;

		if (uidAttacker != uidVictim)
		{
			MMatchObject* pAttacker = MMatchServer::GetInstance()->GetObject(uidAttacker);

			if ((pAttacker) && (pAttacker->CheckAlive()))
			{
				bAttackerCanBeDeity = true;
			}
		}
		else if ((uidAttacker == MUID(0,0)) || (uidAttacker == uidVictim))
		{
			bAttackerCanBeDeity = false;
		}

		if (bAttackerCanBeDeity) m_uidDeity = uidAttacker;
		else m_uidDeity = MUID(0,0);

		RouteAssignDeity();
	}
}

