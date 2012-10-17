#ifndef _MMATCHRULE_DEITY_H
#define _MMATCHRULE_DEITY_H

#include "MMatchRule.h"
#include "MMatchRuleDeathMatch.h"

class MMatchRuleDeity : public MMatchRuleSoloDeath {
protected:
	MUID		m_uidDeity;

	bool CheckKillCount(MMatchObject* pOutObject);
	virtual void OnRoundBegin();
	virtual bool OnCheckRoundFinish();
	void RouteAssignDeity();
	MUID RecommendDeity();
public:
	MMatchRuleDeity(MMatchStage* pStage);
	virtual ~MMatchRuleDeity() { }
	virtual void* CreateRuleInfoBlob();
	virtual void OnEnterBattle(MUID& uidChar);
	virtual void OnLeaveBattle(MUID& uidChar);
	virtual void OnGameKill(const MUID& uidAttacker, const MUID& uidVictim);
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_DEITY; }
	MUID& GetDeity() { return m_uidDeity; }
};

#endif