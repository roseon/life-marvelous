#ifndef _ZRULE_DEITY_H
#define _ZRULE_DEITY_H

#include "ZRule.h"

class ZRuleDeity : public ZRule
{
private:
	MUID	m_uidDeity;
	float	m_fElapsedHealthUpdateTime;
	float   m_fBonusDamage;
	void AssignDeity(MUID& uidDeity);
	virtual void OnUpdate(float fDelta);
	void OnBonus(ZCharacter* pBerserker);
public:
	ZRuleDeity(ZMatch* pMatch);
	virtual ~ZRuleDeity();
	virtual bool OnCommand(MCommand* pCommand);
	virtual void OnResponseRuleInfo(MTD_RuleInfo* pInfo);
	MUID GetDeityUID() const { return m_uidDeity; }
	float GetBonusDamage() const { return m_fBonusDamage; }
};


#endif