#ifndef _ZRULE_CLASSIC_H
#define _ZRULE_CLASSIC_H


#include "ZRule.h"
#include "ZRuleDeathMatch.h"


class ZRuleSoloClassic : public ZRuleSoloDeathMatch
{
public:
	ZRuleSoloClassic(ZMatch* pMatch);
	virtual ~ZRuleSoloClassic();
};


class ZRuleTeamClassic : public ZRuleTeamDeathMatch
{
public:
	ZRuleTeamClassic(ZMatch* pMatch);
	virtual ~ZRuleTeamClassic();
};


//awp
class ZRuleTeamAWP : public ZRuleTeamDeathMatch
{
public:
	ZRuleTeamAWP(ZMatch* pMatch);
	virtual ~ZRuleTeamAWP();
};

//grenade
class ZRuleTeamGrenadeWar : public ZRuleTeamDeathMatch
{
public:
	ZRuleTeamGrenadeWar(ZMatch* pMatch);
	virtual ~ZRuleTeamGrenadeWar();
};

//skillwar
class ZRuleTeamSkillWar : public ZRuleTeamDeathMatch
{
public:
	ZRuleTeamSkillWar(ZMatch* pMatch);
	virtual ~ZRuleTeamSkillWar();
};


#endif