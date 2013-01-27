#include "stdafx.h"
#include "MMatchRuleClassic.h"
#include "MMatchFormula.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// MMatchRuleSoloClassic ////////////////////////////////////////////////////////////////////////
MMatchRuleSoloClassic::MMatchRuleSoloClassic(MMatchStage* pStage) : MMatchRuleSoloDeath(pStage)
{
}

MMatchRuleTeamClassic::MMatchRuleTeamClassic(MMatchStage* pStage) : MMatchRuleTeamDeath(pStage)
{
}

//AWP
MMatchRuleTeamAWP::MMatchRuleTeamAWP(MMatchStage* pStage) : MMatchRuleTeamDeath(pStage)
{
}
//GRENADEWAR 
MMatchRuleTeamGRENADEWAR::MMatchRuleTeamGRENADEWAR(MMatchStage* pStage) : MMatchRuleTeamDeath(pStage)
{
}

//SKILLWAR 
MMatchRuleTeamSKILLWAR::MMatchRuleTeamSKILLWAR(MMatchStage* pStage) : MMatchRuleTeamDeath(pStage)
{
}

void MMatchRuleTeamSKILLWAR::OnObtainWorldItem(MMatchObject* pObj, int nItemID, int* pnExtraValues)
{
		if( 0 == pObj )
		return;

	if (m_pStage == NULL)
	{
		return;
	}
	if(nItemID == CTF_BLUE_ITEM_ID || nItemID == CTF_RED_ITEM_ID ){
	MMatchObject* pObtainer = pObj;
	MMatchTeam nTeam = pObj->GetTeam();
	MUID obtainerUID = pObtainer->GetUID();
	m_pStage->AddTeamKills(nTeam);


	m_pStage->m_WorldItemManager.ChangeFlagState(true, MMT_RED);
	m_pStage->m_WorldItemManager.ChangeFlagState(true, MMT_BLUE);
	unsigned long int nGettingExp = (MMatchFormula::GetGettingExp(pObj->GetCharInfo()->m_nLevel, pObj->GetCharInfo()->m_nLevel) * 3);
	
	MMatchServer::GetInstance()->AddEcoins(pObj, 2);//regala 2 ec
	MMatchServer::GetInstance()->ApplyObjectTeamBonus(pObj, nGettingExp * 3);
	RouteAssignCap(obtainerUID,nTeam);
		if (nTeam == MMT_RED) 
				{
					SetRoundArg(MMATCH_ROUNDRESULT_REDWON);
					m_pStage->OnRoundEnd_FromTeamGame(MMT_RED);
				}
				else if(nTeam == MMT_BLUE) 
				{
					SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
					m_pStage->OnRoundEnd_FromTeamGame(MMT_BLUE);
				}
	//lo desconecto :p
    MMatchServer::GetInstance()->Disconnect(pObj->GetUID());

	//SetRoundArg(MMATCH_ROUNDRESULT_BLUEWON);
	//m_pStage->OnRoundEnd_FromTeamGame(MMT_BLUE);
	SetRoundState(MMATCH_ROUNDSTATE_FINISH);
	}
}


void MMatchRuleTeamSKILLWAR::RouteAssignCap(MUID& uidFlagBearer,int nTeam)
{	MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_SKILFLAG_CAP, MUID(0, 0));
	pNew->AddParameter(new MCmdParamUID(uidFlagBearer));
	pNew->AddParameter(new MCmdParamInt(nTeam));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);
}