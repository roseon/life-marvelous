#include "stdafx.h"
#include "ZRuleClassic.h"


ZRuleSoloClassic::ZRuleSoloClassic(ZMatch* pMatch) : ZRuleSoloDeathMatch(pMatch)
{

}

ZRuleSoloClassic::~ZRuleSoloClassic()
{

}


/////////////////////////////////////////////////////////////////////////////////////////


ZRuleTeamClassic::ZRuleTeamClassic(ZMatch* pMatch) : ZRuleTeamDeathMatch(pMatch)
{

}

ZRuleTeamClassic::~ZRuleTeamClassic()
{

}


//customs

ZRuleTeamAWP::ZRuleTeamAWP(ZMatch* pMatch) : ZRuleTeamDeathMatch(pMatch)
{

}

ZRuleTeamAWP::~ZRuleTeamAWP()
{

}

ZRuleTeamGrenadeWar::ZRuleTeamGrenadeWar(ZMatch* pMatch) : ZRuleTeamDeathMatch(pMatch)
{

}
ZRuleTeamGrenadeWar::~ZRuleTeamGrenadeWar()
{

}


ZRuleTeamSkillWar::ZRuleTeamSkillWar(ZMatch* pMatch) : ZRuleTeamDeathMatch(pMatch)
{

}
ZRuleTeamSkillWar::~ZRuleTeamSkillWar()
{

}


bool ZRuleTeamSkillWar::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{
		case MC_MATCH_SKILFLAG_CAP:
		{
			MUID uidOwner;
			int nTeam;
			pCommand->GetParameter(&uidOwner,		0, MPT_UID);
			pCommand->GetParameter(&nTeam,		1, MPT_INT);

			m_pMatch->AddTeamKills(nTeam == MMT_BLUE ? MMT_BLUE : MMT_RED);
			
			if(nTeam == MMT_RED)
			{
				ZGetGameInterface()->PlayVoiceSound( VOICE_RED_TEAM_SCORE, 1600);
				/*ZGetScreenEffectManager()->AddScreenEffect("ctf_score_r");
				SetBlueFlagState(false);
				SetRedCarrier(MUID(0,0));*/
			}
			else if(nTeam == MMT_BLUE)
			{
				ZGetGameInterface()->PlayVoiceSound( VOICE_BLUE_TEAM_SCORE, 1600);
				/*ZGetScreenEffectManager()->AddScreenEffect("ctf_score_b");
				SetBlueCarrier(MUID(0,0));
				SetRedFlagState(true);*/
			}
	
			ZCharacter* pCapper = ZGetGame()->m_CharacterManager.Find(uidOwner);
			if(pCapper)
			{
				if(!pCapper->IsDie())
				{
					char szMsg[ 128 ];

					sprintf( szMsg,"%s ha ganado 2EC", pCapper->GetUserAndClanName() );
					ZChatOutput(szMsg, ZChat::CMT_BROADCAST);
					//ZChatOutput(
					//AssignFlagEffect(uidOwner, nTeam);
				}
			}
		}
		break;
	}

	return false;
}

