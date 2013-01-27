#ifndef _MMATCHRULE_CLASSIC_H
#define _MMATCHRULE_CLASSIC_H


#include "MMatchRuleDeathMatch.h"

///////////////////////////////////////////////////////////////////////////////////////////////
class MMatchRuleSoloClassic : public MMatchRuleSoloDeath  {
public:
	MMatchRuleSoloClassic(MMatchStage* pStage);
	virtual ~MMatchRuleSoloClassic() { }
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_CLASSIC_SOLO; }
};

///////////////////////////////////////////////////////////////////////////////////////////////
class MMatchRuleTeamClassic : public MMatchRuleTeamDeath {
public:
	MMatchRuleTeamClassic(MMatchStage* pStage);
	virtual ~MMatchRuleTeamClassic()				{}
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_CLASSIC_TEAM; }
};


//AWP
class MMatchRuleTeamAWP : public MMatchRuleTeamDeath {
public:
	MMatchRuleTeamAWP(MMatchStage* pStage);
	virtual ~MMatchRuleTeamAWP()				{}
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_AWP; }
};

//GRENADEWAR 
class MMatchRuleTeamGRENADEWAR : public MMatchRuleTeamDeath {
public:
	MMatchRuleTeamGRENADEWAR(MMatchStage* pStage);
	virtual ~MMatchRuleTeamGRENADEWAR()				{}
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_GRENADEWAR; }
};

//SKILLWAR 
class MMatchRuleTeamSKILLWAR : public MMatchRuleTeamDeath {
public:
	virtual void OnObtainWorldItem(MMatchObject* pObj, int nItemID, int* pnExtraValues);
	virtual void RouteAssignCap(MUID& uidBearer,int nTeam);
	MMatchRuleTeamSKILLWAR(MMatchStage* pStage);
	virtual ~MMatchRuleTeamSKILLWAR()				{}
	virtual MMATCH_GAMETYPE GetGameType() { return MMATCH_GAMETYPE_SKILLWAR; }
};




#endif