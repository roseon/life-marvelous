#include "stdafx.h"
#include "MMatchRuleClassic.h"


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