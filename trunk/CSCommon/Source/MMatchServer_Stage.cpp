#include "stdafx.h"
#include "MMatrix.h"
#include "MMatchServer.h"
#include "MSharedCommandTable.h"
#include "MErrorTable.h"
#include "MBlobArray.h"
#include "MObject.h"
#include "MMatchObject.h"
#include "MMatchItem.h"
#include "MAgentObject.h"
#include "MMatchNotify.h"
#include "Msg.h"
#include "MMatchObjCache.h"
#include "MMatchStage.h"
#include "MMatchTransDataType.h"
#include "MMatchFormula.h"
#include "MMatchConfig.h"
#include "MCommandCommunicator.h"
#include "MMatchShop.h"
#include "MMatchTransDataType.h"
#include "MDebug.h"
#include "MMatchAuth.h"
#include "MMatchStatus.h"
#include "MAsyncDBJob.h"
#include "MVoteDiscussImpl.h"
#include "MUtil.h"
#include "MMatchGameType.h"
#include "MMatchRuleBaseQuest.h"
#include "MMatchRuleQuest.h"
#include "MMatchRuleBerserker.h"
#include "MMatchRuleDuel.h"
#include "MCrashDump.h"

#include "MAsyncDBJob_InsertGamePlayerLog.h"

static bool StageShowInfo(MMatchServer* pServer, const MUID& uidPlayer, const MUID& uidStage, char* pszChat);


MMatchStage* MMatchServer::FindStage(const MUID& uidStage)
{
	MMatchStageMap::iterator i = m_StageMap.find(uidStage);
	if(i==m_StageMap.end()) return NULL;

	MMatchStage* pStage = (*i).second;
	return pStage;
}

bool MMatchServer::StageAdd(MMatchChannel* pChannel, const char* pszStageName, bool bPrivate, const char* pszStagePassword, MUID* pAllocUID, bool bIsAllowNullChannel)
{
	// Ŭ������ pChannel�� NULL�̴�.

	MUID uidStage = m_StageMap.UseUID();
	
	MMatchStage* pStage= new MMatchStage;
	if (pChannel && !pChannel->AddStage(pStage)) {
		delete pStage;
		return false;
	}


	MMATCH_GAMETYPE GameType = MMATCH_GAMETYPE_DEFAULT;
	bool bIsCheckTicket = false;
	DWORD dwTicketID = 0;

	if ( (NULL != pChannel) && MGetServerConfig()->IsUseTicket()) {
		bIsCheckTicket = (pChannel != 0) && pChannel->IsUseTicket() && pChannel->IsTicketChannel();
		dwTicketID = pChannel->GetTicketItemID();

		// Ƽ�� �������� �缳 ä���� ������ Ƽ�� �˻� - Ƽ���� Ŭ���� Ƽ�ϰ� �����ϴ�.
		if ( pChannel->GetChannelType() == MCHANNEL_TYPE_USER) {
			bIsCheckTicket = true;
			dwTicketID = GetChannelMap()->GetClanChannelTicketInfo().m_dwTicketItemID;
		}
	}

	if (!pStage->Create( uidStage, pszStageName, bPrivate, pszStagePassword, bIsAllowNullChannel, GameType, bIsCheckTicket, dwTicketID) ) {
		if (pChannel) {
			pChannel->RemoveStage(pStage);
		}

		delete pStage;
		return false;
	}

	m_StageMap.Insert(uidStage, pStage);

	*pAllocUID = uidStage;

	return true;
}


bool MMatchServer::StageRemove(const MUID& uidStage, MMatchStageMap::iterator* pNextItor)
{
	MMatchStageMap::iterator i = m_StageMap.find(uidStage);
	if(i==m_StageMap.end()) {
		return false;
	}

	MMatchStage* pStage = (*i).second;

	MMatchChannel* pChannel = FindChannel(pStage->GetOwnerChannel());
	if (pChannel) {
		pChannel->RemoveStage(pStage);
	}

	pStage->Destroy();
	delete pStage;

	MMatchStageMap::iterator itorTemp = m_StageMap.erase(i);
	if (pNextItor) *pNextItor = itorTemp;

	return true;
}


bool MMatchServer::StageJoin(const MUID& uidPlayer, const MUID& uidStage)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (!IsEnabledObject(pObj)) return false;

	if (pObj->GetStageUID() != MUID(0,0))
		StageLeave(pObj->GetUID());//, pObj->GetStageUID());

	MMatchChannel* pChannel = FindChannel(pObj->GetChannelUID());
	if (pChannel == NULL) return false;
	if (pChannel->GetChannelType() == MCHANNEL_TYPE_DUELTOURNAMENT) return false;

	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return false;

	int ret = ValidateStageJoin(uidPlayer, uidStage);
	if (ret != MOK) {
		RouteResponseToListener(pObj, MC_MATCH_RESPONSE_STAGE_JOIN, ret);
		return false;
	}
	pObj->OnStageJoin();

	// Cache Add
	MMatchObjectCacheBuilder CacheBuilder;
	CacheBuilder.AddObject(pObj);
	MCommand* pCmdCacheAdd = CacheBuilder.GetResultCmd(MATCHCACHEMODE_ADD, this);
	RouteToStage(pStage->GetUID(), pCmdCacheAdd);

	// Join
	pStage->AddObject(uidPlayer, pObj);
		// �ӽ��ڵ�... �߸��� Ŭ��ID �´ٸ� üũ�Ͽ� �������...20090224 by kammir
	if(pObj->GetCharInfo()->m_ClanInfo.GetClanID() >= 9000000)
		LOG(LOG_FILE, "[UpdateCharClanInfo()] %s's ClanID:%d.", pObj->GetAccountName(), pObj->GetCharInfo()->m_ClanInfo.GetClanID());

	pObj->SetStageUID(uidStage);
	pObj->SetStageState(MOSS_NONREADY);
	pObj->SetTeam(pStage->GetRecommandedTeam());

	// Cast Join
	MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_JOIN), MUID(0,0), m_This);
	pNew->AddParameter(new MCommandParameterUID(uidPlayer));
	pNew->AddParameter(new MCommandParameterUID(pStage->GetUID()));
	pNew->AddParameter(new MCommandParameterUInt(pStage->GetIndex()+1));
	pNew->AddParameter(new MCommandParameterString((char*)pStage->GetName()));
	
	if (pStage->GetState() == STAGE_STATE_STANDBY)  RouteToStage(pStage->GetUID(), pNew);
	else											RouteToListener(pObj, pNew);


	// Cache Update
	CacheBuilder.Reset();
	for (MUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {
		MUID uidObj = (MUID)(*i).first;
		MMatchObject* pScanObj = (MMatchObject*)GetObject(uidObj);
		if (pScanObj) {
			CacheBuilder.AddObject(pScanObj);
		} else {
			LOG(LOG_PROG, "MMatchServer::StageJoin - Invalid ObjectMUID(%u:%u) exist in Stage(%s)\n",
				uidObj.High, uidObj.Low, pStage->GetName());
			pStage->RemoveObject(uidObj);
			return false;
		}
	}
    MCommand* pCmdCacheUpdate = CacheBuilder.GetResultCmd(MATCHCACHEMODE_UPDATE, this);
	RouteToListener(pObj, pCmdCacheUpdate);


	// Cast Master(����)
	MUID uidMaster = pStage->GetMasterUID();
	MCommand* pMasterCmd = CreateCommand(MC_MATCH_STAGE_MASTER, MUID(0,0));
	pMasterCmd->AddParameter(new MCommandParameterUID(uidStage));
	pMasterCmd->AddParameter(new MCommandParameterUID(uidMaster));
	RouteToListener(pObj, pMasterCmd);


#ifdef _QUEST_ITEM
	if (MGetServerConfig()->GetServerMode() == MSM_TEST)
	{
		const MSTAGE_SETTING_NODE* pNode = pStage->GetStageSetting()->GetStageSetting();
		if( 0 == pNode )
		{
			mlog( "MMatchServer::StageJoin - �������� ���� ��� ã�� ����.\n" );
			return false;
		}

		if (MGetGameTypeMgr()->IsQuestDerived(pNode->nGameType))
		{
			MMatchRuleBaseQuest* pRuleQuest = reinterpret_cast< MMatchRuleBaseQuest* >( pStage->GetRule() );
			if( 0 == pRuleQuest )
			{
				mlog( "MMatchServer::StageJoin - ������ ����ȯ ����.\n" );
				return false;
			}

			pRuleQuest->OnChangeCondition();
			//pRuleQuest->OnResponseQL_ToStage( pObj->GetStageUID() );
			// ��ȯ������ ó�� �������� ���νô� ������ ������ ����Ʈ�� ���־ 
			//  ó�� ������ ������ ����Ʈ Ÿ������ �˼��� ���⿡,
			//	Ŭ���̾�Ʈ�� �������� Ÿ���� ����Ʈ������ �ν��ϴ� ��������
			//  �� ������ ��û�� �ϴ� �������� ������. - 05/04/14 by �߱���.
			// pStage->GetRule()->OnResponseSacrificeSlotInfoToStage( uidPlayer );
		}
	}
#endif


	// Cast Character Setting
	StageTeam(uidPlayer, uidStage, pObj->GetTeam());
	StagePlayerState(uidPlayer, uidStage, pObj->GetStageState());


	// ��� �����ڸ� ��������� �ڵ����� ���Ѵ´�. - �°��ӳ� ���� ��û
	if (MMUG_EVENTMASTER == pObj->GetAccountInfo()->m_nUGrade) {
		OnEventChangeMaster(pObj->GetUID());
	}

	return true;
}

bool MMatchServer::StageLeave(const MUID& uidPlayer)//, const MUID& uidStage)
{
	MMatchObject* pObj = GetObject( uidPlayer );
	if( !IsEnabledObject(pObj) ) return false;
	// MMatchStage* pStage = FindStage(uidStage);

	//if(pObj->GetStageUID()!=uidStage)
	//	mlog(" stage leave hack %s (%d, %d) ignore\n", pObj->GetName(), uidPlayer.High, uidPlayer.Low);

	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return false;

	bool bLeaverMaster = false;
	if (uidPlayer == pStage->GetMasterUID()) bLeaverMaster = true;

#ifdef _QUEST_ITEM
	if (MGetServerConfig()->GetServerMode() == MSM_TEST)
	{
		const MSTAGE_SETTING_NODE* pNode = pStage->GetStageSetting()->GetStageSetting();
		if( 0 != pNode )
		{
			if (MGetGameTypeMgr()->IsQuestDerived(pNode->nGameType))
			{
				MMatchRuleBaseQuest* pRuleQuest = reinterpret_cast< MMatchRuleBaseQuest* >( pStage->GetRule() );
				if(pRuleQuest)
				{
					pRuleQuest->PreProcessLeaveStage( uidPlayer );
				} else {
					LOG(LOG_PROG, "StageLeave:: MMatchRule to MMatchRuleBaseQuest FAILED \n");
				}
			}
		}
	}
#endif

	MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_LEAVE), MUID(0,0), m_This);
	pNew->AddParameter(new MCommandParameterUID(uidPlayer));
	// pNew->AddParameter(new MCommandParameterUID(pStage->GetUID()));
	RouteToStage(pStage->GetUID(), pNew);

	pStage->RemoveObject(uidPlayer);

	//MMatchObject* pObj = GetObject(uidPlayer);
	//if (pObj)
	{
		MMatchObjectCacheBuilder CacheBuilder;
		CacheBuilder.AddObject(pObj);
		MCommand* pCmdCache = CacheBuilder.GetResultCmd(MATCHCACHEMODE_REMOVE, this);
		RouteToStage(pStage->GetUID(), pCmdCache);
	}

	// cast Master
	if (bLeaverMaster) StageMaster(pStage->GetUID());

#ifdef _QUEST_ITEM
	// ������ ������������ �����Ŀ� QL�� �ٽ� ����� ��� ��.
	if (MGetServerConfig()->GetServerMode() == MSM_TEST)
	{
		const MSTAGE_SETTING_NODE* pNode = pStage->GetStageSetting()->GetStageSetting();
		if( 0 == pNode )
		{
			mlog( "MMatchServer::StageLeave - �������� ���� ��� ã�� ����.\n" );
			return false;
		}

		if (MGetGameTypeMgr()->IsQuestDerived(pNode->nGameType))
		{
			MMatchRuleBaseQuest* pRuleQuest = reinterpret_cast< MMatchRuleBaseQuest* >( pStage->GetRule() );
			if( 0 == pRuleQuest )
			{
				mlog( "MMatchServer::StageLeave - ������ ����ȯ ����.\n" );
				return false;
			}

			if( STAGE_STATE_STANDBY == pStage->GetState() )
				pRuleQuest->OnChangeCondition();
				//pRuleQuest->OnResponseQL_ToStage( uidStage );
		}
	}
#endif


	return true;
}



DWORD StageEnterBattleExceptionHandler( PEXCEPTION_POINTERS ExceptionInfo )
{
	char szStageDumpFileName[ _MAX_DIR ]= {0,};
	SYSTEMTIME SystemTime;
	GetLocalTime( &SystemTime );
	sprintf( szStageDumpFileName, "Log/StageDump_%d-%d-%d_%d-%d-%d.dmp"
		, SystemTime.wYear
		, SystemTime.wMonth
		, SystemTime.wDay
		, SystemTime.wHour
		, SystemTime.wMinute
		, SystemTime.wSecond );

	return CrashExceptionDump( ExceptionInfo, szStageDumpFileName, true );
}



bool ExceptionTraceStageEnterBattle( MMatchObject* pObj, MMatchStage* pStage )
{
	if( NULL == pObj )
	{
		return false;
	}

	if( NULL == pStage )
	{
		return false;
	}

	__try
	{
		pStage->EnterBattle(pObj);
	}
	__except( StageEnterBattleExceptionHandler(GetExceptionInformation()) )
	{
	/*	mlog( "\nexception : stage enter battle =====================\n" );


		MMatchObject* pMaster = MGetMatchServer()->GetObject( pStage->GetMasterUID() );
		if( NULL != pMaster )  
		{
			if( NULL != pMaster->GetCharInfo() )
			{
				mlog( "stage master cid : %d\n", pMaster->GetCharInfo()->m_nCID );
			}
		}
		else
		{
			mlog( "stage master hass problem.\n" );				
		}
		
		
		if( NULL != pObj->GetCharInfo() )
		{
			mlog( "cmd sender cid : %d\n", pObj->GetCharInfo()->m_nCID );
		}
		else
		{
			mlog( "cmd send char info null point.\n" );
		}

		
		MMatchStageSetting*	pStageSetting = pStage->GetStageSetting();
		if( NULL != pStageSetting )
		{
			mlog( "stage state : %d\n", pStage->GetStageSetting()->GetStageState() );

			const MSTAGE_SETTING_NODE* pExStageSettingNode = pStageSetting->GetStageSetting();
			if( NULL != pExStageSettingNode )
			{
				mlog( "stage name : %s\n", pExStageSettingNode->szMapName );
				mlog( "stage game type : %d\n", pExStageSettingNode->nGameType );
				mlog( "stage max player : %d\n", pExStageSettingNode->nMaxPlayers );
				mlog( "stage current player : %d\n", pStage->GetPlayers() );
				mlog( "stage force entry enable : %d\n", pExStageSettingNode->bForcedEntryEnabled );
				mlog( "stage rule pointer : %x\n", pStage->GetRule() );
			}
		}

		MUIDRefCache::iterator itStage, endStage;
		endStage = pStage->GetObjEnd();
		itStage = pStage->GetObjBegin();
		MMatchObject* pObj = NULL;
		for( ; endStage != itStage; ++itStage )
		{
			pObj = MGetMatchServer()->GetObject( itStage->first );
			if( NULL == pObj )
			{
				mlog( "!!!!stage can't find player!!!!\n" );
				continue;
			}

			mlog( "stage player name : %s\n", pObj->GetName() );
		}

		mlog( "=====================\n\n" );
		*/
		return false;
	}

	return true;
}



bool MMatchServer::StageEnterBattle(const MUID& uidPlayer, const MUID& uidStage)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (!IsEnabledObject(pObj)) return false;
	// MMatchStage* pStage = FindStage(uidStage);
	
	if(pObj->GetStageUID()!=uidStage)
		mlog(" stage enter battle hack %s (%d, %d) ignore\n", pObj->GetName(), uidPlayer.High, uidPlayer.Low);

	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return false;

	pObj->SetPlace(MMP_BATTLE);

	MCommand* pNew = CreateCommand(MC_MATCH_STAGE_ENTERBATTLE, MUID(0,0));
	//pNew->AddParameter(new MCommandParameterUID(uidPlayer));
	//pNew->AddParameter(new MCommandParameterUID(uidStage));

	unsigned char nParam = MCEP_NORMAL;
	if (pObj->IsForcedEntried()) nParam = MCEP_FORCED;
	pNew->AddParameter(new MCommandParameterUChar(nParam));

	void* pPeerArray = MMakeBlobArray(sizeof(MTD_PeerListNode), 1);
	MTD_PeerListNode* pNode = (MTD_PeerListNode*)MGetBlobArrayElement(pPeerArray, 0);
	memset(pNode, 0, sizeof(MTD_PeerListNode));
	
	pNode->uidChar	= pObj->GetUID();
	pNode->dwIP		= pObj->GetIP();
	pNode->nPort	= pObj->GetPort();

	CopyCharInfoForTrans(&pNode->CharInfo, pObj->GetCharInfo(), pObj);
	//���������ӽ��ּ� 	CopyCharBuffInfoForTrans(&pNode->CharBuffInfo, pObj->GetCharInfo(), pObj);

	pNode->ExtendInfo.nPlayerFlags = pObj->GetPlayerFlags();
	if (pStage->GetStageSetting()->IsTeamPlay())	pNode->ExtendInfo.nTeam = (char)pObj->GetTeam();
	else											pNode->ExtendInfo.nTeam = 0;	

	pNew->AddParameter(new MCommandParameterBlob(pPeerArray, MGetBlobArraySize(pPeerArray)));
	MEraseBlobArray(pPeerArray);

	RouteToStage(uidStage, pNew);

	// ��Ʋ ���۽ð� ����
	pObj->GetCharInfo()->m_nBattleStartTime = MMatchServer::GetInstance()->GetGlobalClockCount();
	pObj->GetCharInfo()->m_nBattleStartXP = pObj->GetCharInfo()->m_nXP;

	// ����� �Ŀ� �־�� �Ѵ�.
	return ExceptionTraceStageEnterBattle( pObj, pStage );
}

bool MMatchServer::StageLeaveBattle(const MUID& uidPlayer, bool bGameFinishLeaveBattle, bool bForcedLeave)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (!IsEnabledObject(pObj)) return false;
	if (pObj->GetPlace() != MMP_BATTLE) { return false; }

	// MMatchStage* pStage = FindStage(uidStage);

	//if(pObj->GetStageUID()!=uidStage)
	//	mlog(" stage leave battle hack %s (%d, %d) ignore\n", pObj->GetName(), uidPlayer.High, uidPlayer.Low);

	const MUID uidStage = pObj->GetStageUID();

	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL)
	{	// Ŭ������ ������ �ٳ����� ���������� �������Ƿ� ���⼭ agent�� �����ش�. 
		if (pObj->GetRelayPeer()) {
			MAgentObject* pAgent = GetAgent(pObj->GetAgentUID());
			if (pAgent) {
				MCommand* pCmd = CreateCommand(MC_AGENT_PEER_UNBIND, pAgent->GetCommListener());
				pCmd->AddParameter(new MCmdParamUID(uidPlayer));
				Post(pCmd);
			}
		}

		UpdateCharDBCachingData(pObj);		///< XP, BP, KillCount, DeathCount ĳ�� ������Ʈ
		UpdateCharItemDBCachingData(pObj);	///< Character Item���� ������Ʈ�� �ʿ��� �͵� ������Ʈ
		//CheckSpendableItemCounts(pObj);		///< �׻� UpdateCharItemDBCachingData �ڿ� �־�� �մϴ�.
		
		ProcessCharPlayInfo(pObj);			///< ĳ���� �÷����� ���� ������Ʈ 
		return false;
	}
	else
	{
		// �÷��� ���� ���ǿ��� ����ġ, ��/��, �·�, �ٿ�Ƽ�� �ݿ����� �ʽ��ϴ�. - by kammir 2008.09.19
		// LeaveBattle�� �Ǹ鼭 ĳ���� �����͸� ������Ʈ ���ش�.
		MMatchObjectCacheBuilder CacheBuilder;
		CacheBuilder.Reset();
		for (MUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {
			MMatchObject* pScanObj = (MMatchObject*)(*i).second;
			CacheBuilder.AddObject(pScanObj);
		}
		MCommand* pCmdCacheUpdate = CacheBuilder.GetResultCmd(MATCHCACHEMODE_UPDATE, this);
		RouteToListener(pObj, pCmdCacheUpdate);
	}

	pStage->LeaveBattle(pObj);
	pObj->SetPlace(MMP_STAGE);


	// ������ �ȸ´� �������� üũ
#define LEGAL_ITEMLEVEL_DIFF		3
	bool bIsCorrect = true;
	for (int i = 0; i < MMCIP_END; i++) {
		if (CorrectEquipmentByLevel(pObj, MMatchCharItemParts(i), LEGAL_ITEMLEVEL_DIFF)) {
			bIsCorrect = false;
		}
	}

	if (!bIsCorrect) {
		MCommand* pNewCmd = CreateCommand(MC_MATCH_RESPONSE_RESULT, MUID(0,0));
		pNewCmd->AddParameter(new MCommandParameterInt(MERR_TAKEOFF_ITEM_BY_LEVELDOWN));
		RouteToListener(pObj, pNewCmd);
	}
	
	CheckExpiredItems(pObj);		//< �Ⱓ ���� �������� �ִ��� üũ

	if (pObj->GetRelayPeer()) {
		MAgentObject* pAgent = GetAgent(pObj->GetAgentUID());
		if (pAgent) {
			MCommand* pCmd = CreateCommand(MC_AGENT_PEER_UNBIND, pAgent->GetCommListener());
			pCmd->AddParameter(new MCmdParamUID(uidPlayer));
			Post(pCmd);
		}
	}	

	// ĳ���� �÷����� ���� ������Ʈ 
	ProcessCharPlayInfo(pObj);

	//=======================================================================================================================================
	
	bool bIsLeaveAllBattle = true;
	
	for (MUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {
		MUID uidObj = (MUID)(*i).first;
		MMatchObject* pAllObj = (MMatchObject*)GetObject(uidObj);
		if(NULL == pAllObj) continue;
		if(MMP_STAGE != pAllObj->GetPlace()) { 
			bIsLeaveAllBattle = false; 
			break; 
		}
	}


	if(pStage->IsRelayMap())
	{
		if(bGameFinishLeaveBattle)
		{	// �����̸�, ��Ʋ ����� ���������� ��������
			if(!pStage->m_bIsLastRelayMap)
			{	// �������� �ִٸ� �ٷ� ���� �ʽ��� ó��		

				if( !bForcedLeave ) 
				{
					pObj->SetStageState(MOSS_READY);
				}

				if( bIsLeaveAllBattle ) 
				{					
					OnStageRelayStart(uidStage);
				} 

				MCommand* pNew = CreateCommand(MC_MATCH_STAGE_LEAVEBATTLE_TO_CLIENT, MUID(0,0));
				pNew->AddParameter(new MCommandParameterUID(uidPlayer));
				pNew->AddParameter(new MCommandParameterBool(true));
				RouteToStage(uidStage, pNew);
			}
		}
		else
		{	///< ���� �޴��� ���������� ����		
			MCommand* pNew = CreateCommand(MC_MATCH_STAGE_LEAVEBATTLE_TO_CLIENT, MUID(0,0));
			pNew->AddParameter(new MCommandParameterUID(uidPlayer));
			pNew->AddParameter(new MCommandParameterBool(false));
			RouteToStage(uidStage, pNew);			

			if(bIsLeaveAllBattle) 
			{	///< ��� ���������� �ִٸ� �����̸� ������ �ٽ� ���ش�.
				pStage->m_bIsLastRelayMap = true;//�����̸��� ������
				pStage->GetStageSetting()->SetMapName(MMATCH_MAPNAME_RELAYMAP);
				pStage->SetRelayMapCurrList(pStage->GetRelayMapList());
				pStage->m_RelayMapRepeatCountRemained = pStage->GetRelayMapRepeatCount();
			}
		}
	} 
	else 
	{
		MCommand* pNew = CreateCommand(MC_MATCH_STAGE_LEAVEBATTLE_TO_CLIENT, MUID(0,0));
		pNew->AddParameter(new MCommandParameterUID(uidPlayer));
		pNew->AddParameter(new MCommandParameterBool(false));
		RouteToStage(uidStage, pNew);
	}

	//=======================================================================================================================================

	// �濡�� ������ noready���·� ����ȴ�. 
	// ����� ������ ���������� ��� Ŭ���̾�Ʈ�� ������. - by SungE 2007-06-04
	StagePlayerState( uidPlayer, pStage->GetUID(), pObj->GetStageState() );	
	
	UpdateCharDBCachingData(pObj);		///< XP, BP, KillCount, DeathCount ĳ�� ������Ʈ
	UpdateCharItemDBCachingData(pObj);	///< Character Item���� ������Ʈ�� �ʿ��� �͵� ������Ʈ
	//CheckSpendableItemCounts(pObj);		///< �׻� UpdateCharItemDBCachingData �ڿ� �־�� �մϴ�.

	return true;
}

bool MMatchServer::StageChat(const MUID& uidPlayer, const MUID& uidStage, char* pszChat)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL)	return false;
	MMatchObject* pObj = (MMatchObject*)GetObject(uidPlayer);
	if ((pObj == NULL) || (pObj->GetCharInfo() == NULL)) return false;

	if (pObj->GetAccountInfo()->m_nUGrade == MMUG_CHAT_LIMITED) return false;

//	InsertChatDBLog(uidPlayer, pszChat);

	///< ȫ����(2009.08.04)
	///< ���� �ش� ����ڰ� �ִ� Stage�� ������ Stage�� UID�� �ٸ� ���!
	///< �ٸ� Stage�鿡�Ե� Msg�� ���� �� �ִ� ������ ���� (��ŷ ���α׷� ����)
	if( uidStage != pObj->GetStageUID() )
	{
		//LOG(LOG_FILE,"MMatchServer::StageChat - Different Stage(S:%d, P:%d)", uidStage, pObj->GetStageUID());
		return false;
	}


	MCommand* pCmd = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_CHAT), MUID(0,0), m_This);
	pCmd->AddParameter(new MCommandParameterUID(uidPlayer));
	pCmd->AddParameter(new MCommandParameterUID(uidStage));
	pCmd->AddParameter(new MCommandParameterString(pszChat));
	RouteToStage(uidStage, pCmd);
	return true;
}

bool MMatchServer::StageTeam(const MUID& uidPlayer, const MUID& uidStage, MMatchTeam nTeam)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return false;

	pStage->PlayerTeam(uidPlayer, nTeam);

	MCommand* pCmd = CreateCommand(MC_MATCH_STAGE_TEAM, MUID(0,0));
	pCmd->AddParameter(new MCommandParameterUID(uidPlayer));
	pCmd->AddParameter(new MCommandParameterUID(uidStage));
	pCmd->AddParameter(new MCommandParameterUInt(nTeam));

	RouteToStageWaitRoom(uidStage, pCmd);
	return true;
}

bool MMatchServer::StagePlayerState(const MUID& uidPlayer, const MUID& uidStage, MMatchObjectStageState nStageState)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (!IsEnabledObject(pObj)) return false;
	// MMatchStage* pStage = FindStage(uidStage);
	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return false;

	pStage->PlayerState(uidPlayer, nStageState);
	
	MCommand* pCmd = CreateCommand(MC_MATCH_STAGE_PLAYER_STATE, MUID(0,0));
	pCmd->AddParameter(new MCommandParameterUID(uidPlayer));
	pCmd->AddParameter(new MCommandParameterUID(uidStage));
	pCmd->AddParameter(new MCommandParameterInt(nStageState));
	RouteToStage(uidStage, pCmd);
	return true;
}

bool MMatchServer::StageMaster(const MUID& uidStage)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return false;

	MUID uidMaster = pStage->GetMasterUID();

	MCommand* pCmd = CreateCommand(MC_MATCH_STAGE_MASTER, MUID(0,0));
	pCmd->AddParameter(new MCommandParameterUID(uidStage));
	pCmd->AddParameter(new MCommandParameterUID(uidMaster));
	RouteToStage(uidStage, pCmd);

	return true;
}

void MMatchServer::StageLaunch(const MUID& uidStage)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	ReserveAgent(pStage);

	MCommand* pCmd = CreateCommand(MC_MATCH_STAGE_LAUNCH, MUID(0,0));
	pCmd->AddParameter(new MCmdParamUID(uidStage));
	pCmd->AddParameter(new MCmdParamStr( const_cast<char*>(pStage->GetMapName()) ));
	RouteToStage(uidStage, pCmd);
}

void MMatchServer::StageRelayLaunch(const MUID& uidStage)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	ReserveAgent(pStage);

	for (MUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {
		MUID uidObj = (MUID)(*i).first;
		MMatchObject* pObj = (MMatchObject*)GetObject(uidObj);
		if (pObj) {
			if( pObj->GetStageState() == MOSS_READY) {
				MCommand* pCmd = CreateCommand(MC_MATCH_STAGE_RELAY_LAUNCH, MUID(0,0));
				pCmd->AddParameter(new MCmdParamUID(uidStage));
				pCmd->AddParameter(new MCmdParamStr(const_cast<char*>(pStage->GetMapName())));
				pCmd->AddParameter(new MCmdParamBool(false));
				RouteToListener(pObj, pCmd);
			} else {
				MCommand* pCmd = CreateCommand(MC_MATCH_STAGE_RELAY_LAUNCH, MUID(0,0));
				pCmd->AddParameter(new MCmdParamUID(uidStage));
				pCmd->AddParameter(new MCmdParamStr(const_cast<char*>(pStage->GetMapName())));
				pCmd->AddParameter(new MCmdParamBool(true));
				RouteToListener(pObj, pCmd);
			}
		} else {
			LOG(LOG_PROG, "WARNING(StageRelayLaunch) : Not Existing Obj(%u:%u)\n", uidObj.High, uidObj.Low);
			i=pStage->RemoveObject(uidObj);
			LogObjectCommandHistory(uidObj);
		}
	}
}

void MMatchServer::StageFinishGame(const MUID& uidStage)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	bool bIsRelayMapUnFinish = true;

	if(pStage->IsRelayMap())
	{ // ������ ���϶����� ��Ʋ�� �ٽ� �������ش�. 
		if((int)pStage->m_vecRelayMapsRemained.size() <= 0)
		{	// ���� ���� ������
			int nRepeatCount = (int)pStage->m_RelayMapRepeatCountRemained - 1;
			if(nRepeatCount < 0)
			{
				bIsRelayMapUnFinish = false;

				pStage->m_bIsLastRelayMap = true;//�����̸��� ������				
				nRepeatCount = 0;
				pStage->GetStageSetting()->SetMapName(MMATCH_MAPNAME_RELAYMAP);	//"RelayMap" ����
			}
			pStage->m_RelayMapRepeatCountRemained = (RELAY_MAP_REPEAT_COUNT)nRepeatCount;
			pStage->SetRelayMapCurrList(pStage->GetRelayMapList());
		}

		if(!pStage->m_bIsLastRelayMap) {
			// ó�� ���۽�, Flag�� On�����ش�. 
			if( pStage->IsStartRelayMap() == false ) {
				pStage->SetIsStartRelayMap(true);
			}			

			if((int)pStage->m_vecRelayMapsRemained.size() > 0) { // �������� �ִٸ�
				int nRelayMapIndex = 0;

				if(pStage->GetRelayMapType() == RELAY_MAP_TURN) {	//< ���� ���߿��� ù ��°���� ����(������)
					nRelayMapIndex = 0; 
				} else if(pStage->GetRelayMapType() == RELAY_MAP_RANDOM) {
					nRelayMapIndex = rand() % (int)pStage->m_vecRelayMapsRemained.size();
				}

				if(nRelayMapIndex >= MAX_RELAYMAP_LIST_COUNT) { //< �� ������ 20��
					mlog("StageFinishGame RelayMap Fail RelayMapList MIsCorrect MaxCount[%d] \n", (int)nRelayMapIndex);
					return;
				}

				char* szMapName = (char*)MGetMapDescMgr()->GetMapName(pStage->m_vecRelayMapsRemained[nRelayMapIndex].nMapID);
				if (!szMapName)
				{
					mlog("RelayMapBattleStart Fail MapID[%d] \n", (int)pStage->m_vecRelayMapsRemained[nRelayMapIndex].nMapID);
					return;
				}

				pStage->GetStageSetting()->SetMapName(szMapName);

				// ������ �����̸��� �������ش�.
				vector<RelayMap>::iterator itor = pStage->m_vecRelayMapsRemained.begin();
				for(int i=0 ; nRelayMapIndex > i ; ++itor, ++i);// �ش� �ε������� �̵�
				pStage->m_vecRelayMapsRemained.erase(itor);
			} 
			else {
				mlog("MMatchServer::StageFinishGame::IsRelayMap() - m_vecRelayMapsRemained.size() == 0\n");
			}
		} else {
			pStage->SetIsStartRelayMap(false);
			bIsRelayMapUnFinish = false; // �����̸� ������ ������
		}
	}

	MCommand* pCmd = CreateCommand(MC_MATCH_STAGE_FINISH_GAME, MUID(0,0));
	pCmd->AddParameter(new MCommandParameterUID(uidStage));
	pCmd->AddParameter(new MCommandParameterBool(bIsRelayMapUnFinish));
	RouteToStage(uidStage, pCmd);

	return;
}

MCommand* MMatchServer::CreateCmdResponseStageSetting(const MUID& uidStage)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return NULL;

	MCommand* pCmd = CreateCommand(MC_MATCH_RESPONSE_STAGESETTING, MUID(0,0));
	pCmd->AddParameter(new MCommandParameterUID(pStage->GetUID()));

	MMatchStageSetting* pSetting = pStage->GetStageSetting();

	// Param 1 : Stage Settings
	void* pStageSettingArray = MMakeBlobArray(sizeof(MSTAGE_SETTING_NODE), 1);
	MSTAGE_SETTING_NODE* pNode = (MSTAGE_SETTING_NODE*)MGetBlobArrayElement(pStageSettingArray, 0);
	CopyMemory(pNode, pSetting->GetStageSetting(), sizeof(MSTAGE_SETTING_NODE));
	pCmd->AddParameter(new MCommandParameterBlob(pStageSettingArray, MGetBlobArraySize(pStageSettingArray)));
	MEraseBlobArray(pStageSettingArray);

	// Param 2 : Char Settings
	int nCharCount = (int)pStage->GetObjCount();
	void* pCharArray = MMakeBlobArray(sizeof(MSTAGE_CHAR_SETTING_NODE), nCharCount);
	int nIndex=0;
	for (MUIDRefCache::iterator itor=pStage->GetObjBegin(); itor!=pStage->GetObjEnd(); itor++) {
		MSTAGE_CHAR_SETTING_NODE* pCharNode = (MSTAGE_CHAR_SETTING_NODE*)MGetBlobArrayElement(pCharArray, nIndex++);
		MMatchObject* pObj = (MMatchObject*)(*itor).second;
		pCharNode->uidChar = pObj->GetUID();
		pCharNode->nTeam = pObj->GetTeam();
		pCharNode->nState = pObj->GetStageState();
	}
	pCmd->AddParameter(new MCommandParameterBlob(pCharArray, MGetBlobArraySize(pCharArray)));
	MEraseBlobArray(pCharArray);

	// Param 3 : Stage State
	pCmd->AddParameter(new MCommandParameterInt((int)pStage->GetState()));

	// Param 4 : Stage Master
	pCmd->AddParameter(new MCommandParameterUID(pStage->GetMasterUID()));

	return pCmd;
}



void MMatchServer::OnStageCreate(const MUID& uidChar, char* pszStageName, bool bPrivate, char* pszStagePassword)
{
	MMatchObject* pObj = GetObject(uidChar);
	if (pObj == NULL) return;

	MMatchChannel* pChannel = FindChannel(pObj->GetChannelUID());
	if (pChannel == NULL) return;

	if ((MGetServerConfig()->GetServerMode() == MSM_CLAN) && (pChannel->GetChannelType() == MCHANNEL_TYPE_CLAN)
		&& (pChannel->GetChannelType() == MCHANNEL_TYPE_DUELTOURNAMENT)) {
		return;
	}
	
	MUID uidStage;
	if ((IsAdminGrade(pObj) == false) && (strcmp ("Canal solo para Eventos",pChannel->GetName()) == 0)){
		RouteResponseToListener(pObj, MC_MATCH_RESPONSE_STAGE_CREATE, MERR_CANNOT_CREATE_STAGE);
		return;
	}

	if (!StageAdd(pChannel, pszStageName, bPrivate, pszStagePassword, &uidStage))
	{
		RouteResponseToListener(pObj, MC_MATCH_RESPONSE_STAGE_CREATE, MERR_CANNOT_CREATE_STAGE);
		return;
	}
	StageJoin(uidChar, uidStage);

	MMatchStage* pStage = FindStage(uidStage);
	if (pStage)
		pStage->SetFirstMasterName(pObj->GetCharInfo()->m_szName);
}


//void MMatchServer::OnStageJoin(const MUID& uidChar, const MUID& uidStage)
//{
//	MMatchObject* pObj = GetObject(uidChar);
//	if (pObj == NULL) return;
//
//	MMatchStage* pStage = NULL;
//
//	if (uidStage == MUID(0,0)) {
//		return;
//	} else {
//		pStage = FindStage(uidStage);
//	}
//
//	if (pStage == NULL) {
//		RouteResponseToListener(pObj, MC_MATCH_RESPONSE_STAGE_JOIN, MERR_STAGE_NOT_EXIST);
//		return;
//	}
//
//	if ((IsAdminGrade(pObj) == false) && pStage->IsPrivate())
//	{
//		RouteResponseToListener(pObj, MC_MATCH_RESPONSE_STAGE_JOIN, MERR_CANNOT_JOIN_STAGE_BY_PASSWORD);
//		return;
//	}
//
//	StageJoin(uidChar, pStage->GetUID());
//}

void MMatchServer::OnPrivateStageJoin(const MUID& uidPlayer, const MUID& uidStage, char* pszPassword)
{
	if (strlen(pszPassword) > STAGEPASSWD_LENGTH) return;

	MMatchStage* pStage = NULL;

	if (uidStage == MUID(0,0)) 
	{
		return;
	} 
	else 
	{
		pStage = FindStage(uidStage);
	}

	if (pStage == NULL) 
	{
		MMatchObject* pObj = GetObject(uidPlayer);
		if (pObj != NULL)
		{
			RouteResponseToListener(pObj, MC_MATCH_RESPONSE_STAGE_JOIN, MERR_STAGE_NOT_EXIST);
		}

		return;
	}

	// ���ڳ� �����ڸ� ����..

	bool bSkipPassword = false;

	MMatchObject* pObj = GetObject(uidPlayer);

	if ((pObj == NULL) || (pObj->GetCharInfo() == NULL)) 
		return;

	MMatchUserGradeID ugid = pObj->GetAccountInfo()->m_nUGrade;

	if (ugid == MMUG_DEVELOPER || ugid == MMUG_ADMIN) 
		bSkipPassword = true;

	// ��й��� �ƴϰų� �н����尡 Ʋ���� �н����尡 Ʋ�ȴٰ� �����Ѵ�.
	if(bSkipPassword==false) {
		if ((!pStage->IsPrivate()) || (strcmp(pStage->GetPassword(), pszPassword)))
		{
			MMatchObject* pObj = GetObject(uidPlayer);
			if (pObj != NULL)
			{
				RouteResponseToListener(pObj, MC_MATCH_RESPONSE_STAGE_JOIN, MERR_CANNOT_JOIN_STAGE_BY_PASSWORD);
			}

			return;
		}
	}

	StageJoin(uidPlayer, pStage->GetUID());
}

void MMatchServer::OnStageFollow(const MUID& uidPlayer, const char* pszTargetName)
{
	MMatchObject* pPlayerObj = GetObject(uidPlayer);
	if (pPlayerObj == NULL) return;

	MMatchObject* pTargetObj = GetPlayerByName(pszTargetName);
	if (pTargetObj == NULL) return;

	// �ڱ� �ڽ��� ���� ������ ������� �˻�.
	if (pPlayerObj->GetUID() == pTargetObj->GetUID()) return;

	// ������Ʈ�� �߸��Ǿ� �ִ��� �˻�.
	if (!pPlayerObj->CheckEnableAction(MMatchObject::MMOA_STAGE_FOLLOW)) return;


	// ���� �ٸ� ä������ �˻�.
	if (pPlayerObj->GetChannelUID() != pTargetObj->GetChannelUID()) {

#ifdef _VOTESETTING
		RouteResponseToListener( pPlayerObj, MC_MATCH_RESPONSE_STAGE_FOLLOW, MERR_CANNOT_FOLLOW );
#endif
		return;
	}

	if ((IsAdminGrade(pTargetObj) == true)) {
		NotifyMessage(pPlayerObj->GetUID(), MATCHNOTIFY_GENERAL_USER_NOTFOUND);
		return;
	}

	MMatchStage* pStage = FindStage(pTargetObj->GetStageUID());
	if (pStage == NULL) return;

	// Ŭ���������� ���� �� ����
	if (pStage->GetStageType() != MST_NORMAL) return;

	if (pStage->IsPrivate()==false) {
		if ((pStage->GetStageSetting()->GetForcedEntry()==false) && pStage->GetState() != STAGE_STATE_STANDBY) {
			// Deny Join

#ifdef _VOTESETTING
			RouteResponseToListener( pPlayerObj, MC_MATCH_RESPONSE_STAGE_FOLLOW, MERR_CANNOT_FOLLOW );
#endif
		} else {
			StageJoin(uidPlayer, pTargetObj->GetStageUID());
		}
	}
	else {
		// ���󰡷��� ���� ��й�ȣ�� �ʿ�� �Ұ��� ���󰥼� ����.
		//RouteResponseToListener( pPlayerObj, MC_MATCH_RESPONSE_STAGE_FOLLOW, MERR_CANNOT_FOLLOW_BY_PASSWORD );

		// �ش���� ��й��̸� ��й�ȣ�� �䱸�Ѵ�.
		MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_REQUIRE_PASSWORD), MUID(0,0), m_This);
		pNew->AddParameter(new MCommandParameterUID(pStage->GetUID()));
		pNew->AddParameter(new MCommandParameterString((char*)pStage->GetName()));
		RouteToListener(pPlayerObj, pNew);
	}
}

void MMatchServer::OnStageLeave(const MUID& uidPlayer)//, const MUID& uidStage)
{
	MMatchObject* pObj = GetObject( uidPlayer );
	if( !IsEnabledObject(pObj) ) return;
	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	if( !IsEnabledObject(GetObject(uidPlayer)) )
	{
		return;
	}

	StageLeave(uidPlayer);// , uidStage);
}

void MMatchServer::OnStageRequestPlayerList(const MUID& uidPlayer, const MUID& uidStage)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	// MMatchStage* pStage = FindStage(uidStage);
	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	// ���ο� ���
	MMatchObjectCacheBuilder CacheBuilder;
	CacheBuilder.Reset();
	for (MUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) {
		MMatchObject* pScanObj = (MMatchObject*)(*i).second;
		CacheBuilder.AddObject(pScanObj);
	}
    MCommand* pCmdCacheUpdate = CacheBuilder.GetResultCmd(MATCHCACHEMODE_UPDATE, this);
	RouteToListener(pObj, pCmdCacheUpdate);

	// Cast Master(����)
	MUID uidMaster = pStage->GetMasterUID();
	MCommand* pMasterCmd = CreateCommand(MC_MATCH_STAGE_MASTER, MUID(0,0));
	pMasterCmd->AddParameter(new MCommandParameterUID(uidStage));
	pMasterCmd->AddParameter(new MCommandParameterUID(uidMaster));
	RouteToListener(pObj, pMasterCmd);

	// Cast Character Setting
	StageTeam(uidPlayer, uidStage, pObj->GetTeam());
	StagePlayerState(uidPlayer, uidStage, pObj->GetStageState());
}

void MMatchServer::OnStageEnterBattle(const MUID& uidPlayer, const MUID& uidStage)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	StageEnterBattle(uidPlayer, uidStage);
}

void MMatchServer::OnStageLeaveBattle(const MUID& uidPlayer, bool bGameFinishLeaveBattle)//, const MUID& uidStage)
{
	if( !IsEnabledObject(GetObject(uidPlayer)) )
	{
		return;
	}

	StageLeaveBattle(uidPlayer, bGameFinishLeaveBattle, false);//, uidStage);
}


#include "CMLexicalAnalyzer.h"
// ���� �ӽ��ڵ�
bool StageKick(MMatchServer* pServer, const MUID& uidPlayer, const MUID& uidStage, char* pszChat)
{
	MMatchObject* pChar = pServer->GetObject(uidPlayer);
	if (pChar == NULL)	return false;
	MMatchStage* pStage = pServer->FindStage(uidStage);
	if (pStage == NULL) return false;
	if (uidPlayer != pStage->GetMasterUID()) return false;

	bool bResult = false;
	CMLexicalAnalyzer lex;
	lex.Create(pszChat);

	if (lex.GetCount() >= 1) {
		char* pszCmd = lex.GetByStr(0);
		if (pszCmd) {
			if (stricmp(pszCmd, "/kick") == 0) {
				if (lex.GetCount() >= 2) {
					char* pszTarget = lex.GetByStr(1);
					if (pszTarget) {
						for (MUIDRefCache::iterator itor = pStage->GetObjBegin(); 
							itor != pStage->GetObjEnd(); ++itor)
						{
							MMatchObject* pTarget = (MMatchObject*)((*itor).second);
							if (stricmp(pszTarget, pTarget->GetName()) == 0) {
								if (pTarget->GetPlace() != MMP_BATTLE) {
									pServer->StageLeave(pTarget->GetUID());//, uidStage);
									bResult = true;
								}
								break;
							}
						}
					}
				}
			}	// Kick
		}
	}

	lex.Destroy();
	return bResult;
}

// ����Ȯ�� �ӽ��ڵ�
bool StageShowInfo(MMatchServer* pServer, const MUID& uidPlayer, const MUID& uidStage, char* pszChat)
{
	MMatchObject* pChar = pServer->GetObject(uidPlayer);
	if (pChar == NULL)	return false;
	MMatchStage* pStage = pServer->FindStage(uidStage);
	if (pStage == NULL) return false;
	if (uidPlayer != pStage->GetMasterUID()) return false;

	bool bResult = false;
	CMLexicalAnalyzer lex;
	lex.Create(pszChat);

	if (lex.GetCount() >= 1) {
		char* pszCmd = lex.GetByStr(0);
		if (pszCmd) {
			if (stricmp(pszCmd, "/showinfo") == 0) {
				char szMsg[256]="";
				sprintf(szMsg, "FirstMaster : (%s)", pStage->GetFirstMasterName());
				pServer->Announce(pChar, szMsg);
				bResult = true;
			}	// ShowInfo
		}
	}

	lex.Destroy();
	return bResult;
}
void MMatchServer::OnStageChat(const MUID& uidPlayer, const MUID& uidStage, char* pszChat)
{
	// RAONHAJE : ���� �ӽ��ڵ�
	if (pszChat[0] == '/') {
		if (StageKick(this, uidPlayer, uidStage, pszChat))
			return;
		if (StageShowInfo(this, uidPlayer, uidStage, pszChat))
			return;
	}

	StageChat(uidPlayer, uidStage, pszChat);
}

void MMatchServer::OnStageStart(const MUID& uidPlayer, const MUID& uidStage, int nCountdown)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	if (pStage->GetMasterUID() != uidPlayer) return;

	if (pStage->StartGame(MGetServerConfig()->IsUseResourceCRC32CacheCheck()) == true) {
		StageRelayMapBattleStart(uidPlayer, uidStage);

		MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_START), MUID(0,0), m_This);
		pNew->AddParameter(new MCommandParameterUID(uidPlayer));
		pNew->AddParameter(new MCommandParameterUID(uidStage));
		pNew->AddParameter(new MCommandParameterInt(min(nCountdown,3)));
		RouteToStage(uidStage, pNew);

		// ��� �α׸� �����.
		SaveGameLog(uidStage);
	}
}

void MMatchServer::OnStageRelayStart(const MUID& uidStage)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	
	if (pStage->StartRelayGame(MGetServerConfig()->IsUseResourceCRC32CacheCheck()) == true) {
		// ��� �α׸� �����.
		SaveGameLog(uidStage);
	}
}

void MMatchServer::OnStartStageList(const MUID& uidComm)
{
	MMatchObject* pObj = GetPlayerByCommUID(uidComm);
	if (pObj == NULL) return;

	pObj->SetStageListTransfer(true);
}

void MMatchServer::OnStopStageList(const MUID& uidComm)
{
	MMatchObject* pObj = GetPlayerByCommUID(uidComm);
	if (pObj == NULL) return;

	pObj->SetStageListTransfer(false);
}

void MMatchServer::OnStagePlayerState(const MUID& uidPlayer, const MUID& uidStage, MMatchObjectStageState nStageState)
{
	StagePlayerState(uidPlayer, uidStage, nStageState);
}


void MMatchServer::OnStageTeam(const MUID& uidPlayer, const MUID& uidStage, MMatchTeam nTeam)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	MMatchObject* pChar = GetObject(uidPlayer);
	if (pChar == NULL) return;

	StageTeam(uidPlayer, uidStage, nTeam);
}

void MMatchServer::OnStageMap(const MUID& uidStage, char* pszMapName)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	if (pStage->GetState() != STAGE_STATE_STANDBY) return;	// �����¿����� �ٲܼ� �ִ�
	if (strlen(pszMapName) < 2) return;

	pStage->SetMapName( pszMapName );
	pStage->SetIsRelayMap(strcmp(MMATCH_MAPNAME_RELAYMAP, pszMapName) == 0);
	
	MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_MAP), MUID(0,0), m_This);
	pNew->AddParameter(new MCommandParameterUID(uidStage));
	pNew->AddParameter(new MCommandParameterString(pStage->GetMapName()));

	if ( MGetGameTypeMgr()->IsQuestDerived( pStage->GetStageSetting()->GetGameType()))
	{
		MMatchRuleBaseQuest* pQuest = reinterpret_cast< MMatchRuleBaseQuest* >( pStage->GetRule() );
		pQuest->RefreshStageGameInfo();
	}

    RouteToStage(uidStage, pNew);
}

void MMatchServer::StageRelayMapBattleStart(const MUID& uidPlayer, const MUID& uidStage)
{// �����̸� �����ϰ� ���� ���� ��ư ������ ������ �����Ѵ�
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	if (pStage->GetMasterUID() != uidPlayer) return;
	if(!pStage->IsRelayMap()) return;
	
	// ���� ó�����۽� �ʱ�ȭ ���ֱ�
	pStage->InitCurrRelayMap();

	if (pStage->m_vecRelayMapsRemained.empty()) return;

	if((int)pStage->m_vecRelayMapsRemained.size() > MAX_RELAYMAP_LIST_COUNT)
	{// �� ������ 20�� �ʰ��ϸ� ����
		mlog("RelayMapBattleStart Fail RelayMapList MIsCorrect OverCount[%d] \n", (int)pStage->m_vecRelayMapsRemained.size());
		return;
	}

	if (pStage->m_vecRelayMapsRemained.size() != pStage->GetRelayMapListCount())
	{
		mlog("m_vecRelayMapsRemained[%d] != GetRelayMapListCount[%d]\n", (int)pStage->m_vecRelayMapsRemained.size(), pStage->GetRelayMapListCount());
		return;
	}

	// ó�� ������ ���� ���Ѵ�
	int nRelayMapIndex = 0;
	if(pStage->GetRelayMapType() == RELAY_MAP_TURN )
		nRelayMapIndex = 0; // �������߿��� ó����° ���� ����(������)
	else if(pStage->GetRelayMapType() == RELAY_MAP_RANDOM)
		nRelayMapIndex = rand() % int(pStage->m_vecRelayMapsRemained.size());

	if(MMATCH_MAP_RELAYMAP == pStage->m_vecRelayMapsRemained[nRelayMapIndex].nMapID)
	{
		mlog("RelayMapBattleStart Fail Type[%d], RoundCount[Curr:%d][%d], ListCount[Curr:%d][%d] \n",  
			pStage->GetRelayMapType(), pStage->m_RelayMapRepeatCountRemained, pStage->GetRelayMapRepeatCount(), (int)pStage->m_vecRelayMapsRemained.size(), pStage->GetRelayMapListCount());
		return;
	}

	char* szMapName = (char*)MGetMapDescMgr()->GetMapName(pStage->m_vecRelayMapsRemained[nRelayMapIndex].nMapID);
	if (!szMapName)
	{
		mlog("RelayMapBattleStart Fail MapID[%d] \n", (int)pStage->m_vecRelayMapsRemained[nRelayMapIndex].nMapID);
		return;
	}

	pStage->GetStageSetting()->SetMapName(szMapName);

	// ������ �����̸��� �������ش�.
	vector<RelayMap>::iterator itor = pStage->m_vecRelayMapsRemained.begin();
	for(int i=0 ; nRelayMapIndex > i ; ++itor, ++i);// �ش� �ε������� �̵�
	pStage->m_vecRelayMapsRemained.erase(itor);
}

void MMatchServer::OnStageRelayMapElementUpdate(const MUID& uidStage, int nType, int nRepeatCount)
{
	MMatchStage* pStage = FindStage(uidStage);
	pStage->SetRelayMapType((RELAY_MAP_TYPE)nType);
	pStage->SetRelayMapRepeatCount((RELAY_MAP_REPEAT_COUNT)nRepeatCount);

	MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_RELAY_MAP_ELEMENT_UPDATE), MUID(0,0), m_This);
	pNew->AddParameter(new MCommandParameterUID(uidStage));
	pNew->AddParameter(new MCommandParameterInt((int)pStage->GetRelayMapType()));
	pNew->AddParameter(new MCommandParameterInt((int)pStage->GetRelayMapRepeatCount()));
	RouteToStage(uidStage, pNew);
}

void MMatchServer::OnStageRelayMapListUpdate(const MUID& uidStage, int nRelayMapType, int nRelayMapRepeatCount, void* pRelayMapListBlob)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	if(!pStage->IsRelayMap()) return;
	if (pStage->GetState() != STAGE_STATE_STANDBY) return;	// �����¿����� �ٲܼ� �ִ�

	// �����̸� ������ ������ ���������� ����
	RelayMap relayMapList[MAX_RELAYMAP_LIST_COUNT];
	for (int i = 0; i < MAX_RELAYMAP_LIST_COUNT; i++)
		relayMapList[i].nMapID = -1;
	int nRelayMapListCount = MGetBlobArrayCount(pRelayMapListBlob);
	if(nRelayMapListCount > MAX_RELAYMAP_LIST_COUNT)
		nRelayMapListCount = MAX_RELAYMAP_LIST_COUNT;
	for (int i = 0; i < nRelayMapListCount; i++)
	{
		MTD_RelayMap* pRelayMap = (MTD_RelayMap*)MGetBlobArrayElement(pRelayMapListBlob, i);
		if(!MGetMapDescMgr()->MIsCorrectMap(pRelayMap->nMapID))
		{
			mlog("OnStageRelayMapListUpdate Fail MIsCorrectMap ID[%d] \n", (int)pRelayMap->nMapID);
			break;
		}
		relayMapList[i].nMapID = pRelayMap->nMapID;
	}

	pStage->SetRelayMapType((RELAY_MAP_TYPE)nRelayMapType);
	pStage->SetRelayMapRepeatCount((RELAY_MAP_REPEAT_COUNT)nRelayMapRepeatCount);
	pStage->SetRelayMapList(relayMapList);
	pStage->InitCurrRelayMap();


	// ���� �����, �ʸ���Ʈ ����
	void* pRelayMapListBlob = MMakeBlobArray(sizeof(MTD_RelayMap), pStage->GetRelayMapListCount());
	RelayMap RelayMapList[MAX_RELAYMAP_LIST_COUNT];
	memcpy(RelayMapList, pStage->GetRelayMapList(), sizeof(RelayMap)*MAX_RELAYMAP_LIST_COUNT);
	for (int i = 0; i < pStage->GetRelayMapListCount(); i++)
	{
		MTD_RelayMap* pRelayMapList = (MTD_RelayMap*)MGetBlobArrayElement(pRelayMapListBlob, i);
		pRelayMapList->nMapID = RelayMapList[i].nMapID;
	}

	// ������ ���� �����̸� ������ ����鿡�� ����
	MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_RELAY_MAP_INFO_UPDATE), MUID(0,0), m_This);
	pNew->AddParameter(new MCommandParameterUID(uidStage));
	pNew->AddParameter(new MCommandParameterInt((int)pStage->GetRelayMapType()));
	pNew->AddParameter(new MCommandParameterInt((int)pStage->GetRelayMapRepeatCount()));
	pNew->AddParameter(new MCommandParameterBlob(pRelayMapListBlob, MGetBlobArraySize(pRelayMapListBlob)));
	RouteToStage(uidStage, pNew);
}
void MMatchServer::OnStageRelayMapListInfo(const MUID& uidStage, const MUID& uidChar)
{
	MMatchStage* pStage = FindStage(uidStage);
	if(pStage == NULL) return;
	if(!pStage->IsRelayMap()) return;
	MMatchObject* pObj = GetObject(uidChar);
	if (pObj == NULL) return;
	// �������϶� ������ ó�� ������(�����̸� �ۼ����ϼ��� ����)
	if(pStage->GetState() == STAGE_STATE_STANDBY && pStage->GetMasterUID() == uidChar) return;	

	// ���� �����, �ʸ���Ʈ ����
	void* pRelayMapListBlob = MMakeBlobArray(sizeof(MTD_RelayMap), pStage->GetRelayMapListCount());
	RelayMap RelayMapList[MAX_RELAYMAP_LIST_COUNT];
	memcpy(RelayMapList, pStage->GetRelayMapList(), sizeof(RelayMap)*MAX_RELAYMAP_LIST_COUNT);
	for (int i = 0; i < pStage->GetRelayMapListCount(); i++)
	{
		MTD_RelayMap* pRelayMapList = (MTD_RelayMap*)MGetBlobArrayElement(pRelayMapListBlob, i);
		pRelayMapList->nMapID = RelayMapList[i].nMapID;
	}
	MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_RELAY_MAP_INFO_UPDATE), MUID(0,0), m_This);
	pNew->AddParameter(new MCommandParameterUID(uidStage));
	pNew->AddParameter(new MCommandParameterInt((int)pStage->GetRelayMapType()));
	pNew->AddParameter(new MCommandParameterInt((int)pStage->GetRelayMapRepeatCount()));
	pNew->AddParameter(new MCommandParameterBlob(pRelayMapListBlob, MGetBlobArraySize(pRelayMapListBlob)));
	MEraseBlobArray(pRelayMapListBlob);

	RouteToListener(pObj, pNew); // ������ �����̸� �����߿� ������Ʈ�� �������� ���� �ɼ��� ����
}

void MMatchServer::OnStageSetting(const MUID& uidPlayer, const MUID& uidStage, void* pStageBlob, int nStageCount)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	if (pStage->GetState() != STAGE_STATE_STANDBY) return;	// �����¿����� �ٲܼ� �ִ�
	if (nStageCount <= 0) return;

	// validate
	MMatchObject* pObj = GetObject(uidPlayer);
	if (!IsEnabledObject(pObj)) {
		mlog(" stage setting invalid object (%d, %d) ignore\n", uidPlayer.High, uidPlayer.Low);
		return;
	}

	if( pObj->GetStageUID()!=uidStage ||  nStageCount!=1 ||
		MGetBlobArraySize(pStageBlob) != (sizeof(MSTAGE_SETTING_NODE)+sizeof(int)*2) )
	{
		mlog(" stage setting hack %s (%d, %d) ignore\n", pObj->GetName(), uidPlayer.High, uidPlayer.Low);
		LogObjectCommandHistory( uidPlayer );
		return;
	}

	// �����̰ų� ��ڰ� �ƴѵ� ������ �ٲٸ� �׳� ����
	if (pStage->GetMasterUID() != uidPlayer)
	{
		MMatchObject* pObjMaster = GetObject(uidPlayer);
		if (!IsAdminGrade(pObjMaster)) return;
	}


	MSTAGE_SETTING_NODE* pNode = (MSTAGE_SETTING_NODE*)MGetBlobArrayElement(pStageBlob, 0);

	// let's refactor
	if( (pNode->nGameType < MMATCH_GAMETYPE_DEATHMATCH_SOLO) || (pNode->nGameType >= MMATCH_GAMETYPE_MAX)) {
		mlog(" stage setting game mode hack %s (%d, %d) ignore\n", pObj->GetName(), uidPlayer.High, uidPlayer.Low);
		LogObjectCommandHistory( uidPlayer );

		// ��� ������.
//		pObj->SetInvalidStageSettingDisconnectWaitInfo();
		pObj->DisconnectHacker( MMHT_INVALIDSTAGESETTING );

		return;
	}

	// �����̹��� ��Ȱ�� �����ε� �����̹� ��û��
	if( MGetServerConfig()->IsEnabledSurvivalMode()==false && pNode->nGameType==MMATCH_GAMETYPE_SURVIVAL) {
		mlog(" stage setting game mode hack %s (%d, %d) ignore\n", pObj->GetName(), uidPlayer.High, uidPlayer.Low);
		LogObjectCommandHistory( uidPlayer );
		pObj->DisconnectHacker( MMHT_INVALIDSTAGESETTING );
		return;
	}

	// �⺻������ �ִ� �ο��� STAGE_BASIC_MAX_PLAYERCOUNT�� ������ STAGE_BASIC_MAX_PLAYERCOUNT�� ������.
	// ���� �۾��� �����ϸ��� �� ���ӿ� �´� �ο����� ������ ��. - by SungE 2007-05-14
	if( STAGE_MAX_PLAYERCOUNT < pNode->nMaxPlayers )
		pNode->nMaxPlayers = STAGE_MAX_PLAYERCOUNT;

	// �� �̻��� ���� ������ �Ұ��� �ϴ�. ������ �����Ѵ�. - By SungE 2007-11-07
	if( STAGE__MAX_ROUND < pNode->nRoundMax )
		pNode->nRoundMax = STAGE__MAX_ROUND;

	MMatchStageSetting* pSetting = pStage->GetStageSetting();
	MMatchChannel* pChannel = FindChannel(pStage->GetOwnerChannel());

	bool bCheckChannelRule = true;

	if (QuestTestServer())
	{
		if (MGetGameTypeMgr()->IsQuestDerived(pNode->nGameType))
		{
			bCheckChannelRule = false;
		}
	}

	if ((pChannel) && (bCheckChannelRule))
	{
		// ������ �� �ִ� ��, ����Ÿ������ üũ
		MChannelRule* pRule = MGetChannelRuleMgr()->GetRule(pChannel->GetRuleType());
		if (pRule)
		{
			if (!pRule->CheckGameType(pNode->nGameType))
			{
				pNode->nGameType = MMATCH_GAMETYPE_DEATHMATCH_SOLO;
			}

			bool bDuelMode = false;
			bool bCTFMode = false;

			if ( pNode->nGameType == MMATCH_GAMETYPE_DUEL)
			{
					bDuelMode = true;
			}

			if ( pNode->nGameType == MMATCH_GAMETYPE_CTF){
                 bCTFMode = true;
			}

			if (!pRule->CheckCTFMap(pNode->nMapIndex) && bCTFMode)
			{
				strcpy(pNode->szMapName, MGetMapDescMgr()->GetMapName(MMATCH_MAP_MANSION));
				pNode->nMapIndex = 0;
			}
			if (!pRule->CheckMap(pNode->nMapIndex, bDuelMode) && !bCTFMode)
			{
				strcpy(pNode->szMapName, MGetMapDescMgr()->GetMapName(MMATCH_MAP_MANSION));
				pNode->nMapIndex = 0;
			}
			else if(!bCTFMode)
			{
				strcpy(pNode->szMapName, pSetting->GetMapName());
				pNode->nMapIndex = pSetting->GetMapIndex();
			}
		}
	}

	MMATCH_GAMETYPE nLastGameType = pSetting->GetGameType();

	// ����Ʈ ����̸� ������ ���ԺҰ�, �ִ��ο� 4������ �����Ѵ�.
	if (MGetGameTypeMgr()->IsQuestDerived(pNode->nGameType))
	{
		if (pNode->bForcedEntryEnabled == true) pNode->bForcedEntryEnabled = false;
		pNode->nMaxPlayers = STAGE_QUEST_MAX_PLAYER;
		pNode->nLimitTime = STAGESETTING_LIMITTIME_UNLIMITED;


		// ����Ʈ ������ �ƴѵ� ����Ʈ �����̸� �ַε�����ġ�� �ٲ۴�.
		if (!QuestTestServer())
		{
			pNode->nGameType = MMATCH_GAMETYPE_DEATHMATCH_SOLO;
		}
	}

	// ����Ʈ ��忴�ٰ� �ٸ� ��尡 �Ǹ� '���ԺҰ�'�� ������� ����
	if (MGetGameTypeMgr()->IsQuestDerived( nLastGameType ) == true &&
		MGetGameTypeMgr()->IsQuestDerived( pNode->nGameType ) == false)
		pNode->bForcedEntryEnabled = true;

	if (!MGetGameTypeMgr()->IsTeamGame(pNode->nGameType))
	{
		pNode->bAutoTeamBalancing = true;
	}

	// �����̸� ����
	pStage->SetIsRelayMap(strcmp(MMATCH_MAPNAME_RELAYMAP, pNode->szMapName) == 0);
	pStage->SetIsStartRelayMap(false);

	if(!pStage->IsRelayMap())
	{	// �����̸��� �ƴϸ� �⺻���� �ʱ�ȭ ���ش�.
		pNode->bIsRelayMap = pStage->IsRelayMap();
		pNode->bIsStartRelayMap = pStage->IsStartRelayMap();
		for (int i=0; i<MAX_RELAYMAP_LIST_COUNT; ++i)
			pNode->MapList[i].nMapID = -1;
		pNode->nRelayMapListCount = 0;
		pNode->nRelayMapType = RELAY_MAP_TURN;
		pNode->nRelayMapRepeatCount = RELAY_MAP_3REPEAT;
	}

	pSetting->UpdateStageSetting(pNode);
	pStage->ChangeRule(pNode->nGameType);


	MCommand* pCmd = CreateCmdResponseStageSetting(uidStage);
	RouteToStage(uidStage, pCmd);


	// ���� ��尡 ����Ǿ������
	if (nLastGameType != pSetting->GetGameType())
	{
		char szNewMap[ MAPNAME_LENGTH ] = {0};

		if (MGetGameTypeMgr()->IsQuestDerived( nLastGameType ) == false &&
			MGetGameTypeMgr()->IsQuestDerived( pSetting->GetGameType() ) == true)
		{
//			OnStageMap(uidStage, GetQuest()->GetSurvivalMapInfo(MSURVIVAL_MAP(0))->szName);
//			OnStageMap(uidStage, pSetting->GetMapName());
			OnStageMap(uidStage, MMATCH_DEFAULT_STAGESETTING_MAPNAME);

			MMatchRuleBaseQuest* pQuest = reinterpret_cast< MMatchRuleBaseQuest* >( pStage->GetRule());
			pQuest->RefreshStageGameInfo();
		}
		else if ( (nLastGameType != MMATCH_GAMETYPE_DUEL) && ( pSetting->GetGameType() == MMATCH_GAMETYPE_DUEL))
		{
			strcpy( szNewMap, MGetMapDescMgr()->GetMapName( MMATCH_MAP_HALL));
			OnStageMap(uidStage, szNewMap);
		}
		else if ( ((nLastGameType == MMATCH_GAMETYPE_QUEST) || (nLastGameType == MMATCH_GAMETYPE_SURVIVAL) || (nLastGameType == MMATCH_GAMETYPE_DUEL)) &&
			      ((pSetting->GetGameType() != MMATCH_GAMETYPE_QUEST) && (pSetting->GetGameType() != MMATCH_GAMETYPE_SURVIVAL) && ( pSetting->GetGameType() != MMATCH_GAMETYPE_DUEL)))
		{
			strcpy( szNewMap, MGetMapDescMgr()->GetMapName( MMATCH_MAP_MANSION));
			OnStageMap(uidStage, szNewMap);
		}
	}
}

void MMatchServer::OnRequestStageSetting(const MUID& uidComm, const MUID& uidStage)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	MCommand* pCmd = CreateCmdResponseStageSetting(uidStage);
	pCmd->m_Receiver = uidComm;
	Post(pCmd);

	// �� ������ �����̸��̸� ó�����ش�.
	OnStageRelayMapListInfo(uidStage, uidComm);

	MMatchObject* pChar = GetObject(uidComm);
	if (pChar && (MMUG_EVENTMASTER == pChar->GetAccountInfo()->m_nUGrade)) 	{
		// �̺�Ʈ �����Ϳ��� ó�� �游����� ����� �˷��ش�
		StageShowInfo(this, uidComm, uidStage, "/showinfo");
	}
}

void MMatchServer::OnRequestPeerList(const MUID& uidChar, const MUID& uidStage)
{
	ResponsePeerList(uidChar, uidStage);
}

void MMatchServer::OnRequestGameInfo(const MUID& uidChar, const MUID& uidStage)
{
	ResponseGameInfo(uidChar, uidStage);
}

void MMatchServer::ResponseGameInfo(const MUID& uidChar, const MUID& uidStage)
{
	MMatchStage* pStage = FindStage(uidStage); if (pStage == NULL) return;
	MMatchObject* pObj = GetObject(uidChar); if (pObj == NULL) return;
	if (pStage->GetRule() == NULL) return;

	MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_GAME_INFO, MUID(0,0));
	pNew->AddParameter(new MCommandParameterUID(pStage->GetUID()));

	// ��������
	void* pGameInfoArray = MMakeBlobArray(sizeof(MTD_GameInfo), 1);
	MTD_GameInfo* pGameItem = (MTD_GameInfo*)MGetBlobArrayElement(pGameInfoArray, 0);
	memset(pGameItem, 0, sizeof(MTD_GameInfo));
	
	if (pStage->GetStageSetting()->IsTeamPlay())
	{
		pGameItem->nRedTeamScore = static_cast<char>(pStage->GetTeamScore(MMT_RED));
		pGameItem->nBlueTeamScore = static_cast<char>(pStage->GetTeamScore(MMT_BLUE));

		pGameItem->nRedTeamKills = static_cast<short>(pStage->GetTeamKills(MMT_RED));
		pGameItem->nBlueTeamKills = static_cast<short>(pStage->GetTeamKills(MMT_BLUE));
	}

	pNew->AddParameter(new MCommandParameterBlob(pGameInfoArray, MGetBlobArraySize(pGameInfoArray)));
	MEraseBlobArray(pGameInfoArray);

	// ������
	void* pRuleInfoArray = NULL;
	if (pStage->GetRule())
		pRuleInfoArray = pStage->GetRule()->CreateRuleInfoBlob();
	if (pRuleInfoArray == NULL)
		pRuleInfoArray = MMakeBlobArray(0, 0);
	pNew->AddParameter(new MCommandParameterBlob(pRuleInfoArray, MGetBlobArraySize(pRuleInfoArray)));
	MEraseBlobArray(pRuleInfoArray);

	// Battle�� �� ����� List�� �����.
	int nPlayerCount = pStage->GetObjInBattleCount();

	void* pPlayerItemArray = MMakeBlobArray(sizeof(MTD_GameInfoPlayerItem), nPlayerCount);
	int nIndex=0;
	for (MUIDRefCache::iterator itor=pStage->GetObjBegin(); itor!=pStage->GetObjEnd(); itor++) 
	{
		MMatchObject* pObj = (MMatchObject*)(*itor).second;
		if (pObj->GetEnterBattle() == false) continue;

		MTD_GameInfoPlayerItem* pPlayerItem = (MTD_GameInfoPlayerItem*)MGetBlobArrayElement(pPlayerItemArray, nIndex++);
		pPlayerItem->uidPlayer = pObj->GetUID();
		pPlayerItem->bAlive = pObj->CheckAlive();
		pPlayerItem->nKillCount = pObj->GetAllRoundKillCount();
		pPlayerItem->nDeathCount = pObj->GetAllRoundDeathCount();
	}
	pNew->AddParameter(new MCommandParameterBlob(pPlayerItemArray, MGetBlobArraySize(pPlayerItemArray)));
	MEraseBlobArray(pPlayerItemArray);

	RouteToListener(pObj, pNew);
}

void MMatchServer::OnMatchLoadingComplete(const MUID& uidPlayer, int nPercent)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	MCommand* pCmd = CreateCommand(MC_MATCH_LOADING_COMPLETE, MUID(0,0));
	pCmd->AddParameter(new MCmdParamUID(uidPlayer));
	pCmd->AddParameter(new MCmdParamInt(nPercent));
	RouteToStage(pObj->GetStageUID(), pCmd);	
}


void MMatchServer::OnGameRoundState(const MUID& uidStage, int nState, int nRound)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	pStage->RoundStateFromClient(uidStage, nState, nRound);
}


void MMatchServer::OnDuelSetObserver(const MUID& uidChar)
{
	MMatchObject* pObj = GetObject(uidChar);
	if (pObj == NULL) return;

	MCommand* pCmd = CreateCommand(MC_MATCH_SET_OBSERVER, MUID(0,0));
	pCmd->AddParameter(new MCmdParamUID(uidChar));
	RouteToBattle(pObj->GetStageUID(), pCmd);
}

void MMatchServer::OnRequestSpawn(const MUID& uidChar, const MVector& pos, const MVector& dir)
{
	MMatchObject* pObj = GetObject(uidChar);
	if (pObj == NULL) return;

	// Do Not Spawn when AdminHiding
	if (IsAdminGrade(pObj) && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) return;


	// ������ �׾��� �ð��� ���� �������� ��û�� �ð� ���̿� 2�� �̻��� �ð��� �־����� �˻��Ѵ�.
	DWORD dwTime = timeGetTime() - pObj->GetLastSpawnTime();	
	if ( dwTime < RESPAWN_DELAYTIME_AFTER_DYING_MIN) return;
	pObj->SetLastSpawnTime(timeGetTime());

	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;
	if ( (pStage->GetRule()->GetRoundState() != MMATCH_ROUNDSTATE_PREPARE) && (!pObj->IsEnabledRespawnDeathTime(GetTickTime())) )
		 return;

	MMatchRule* pRule = pStage->GetRule();					// �̷� ���� �ڵ�� ������ �ȵ����� -_-; ����Ÿ�� ���� ����ó��.
	MMATCH_GAMETYPE gameType = pRule->GetGameType();		// �ٸ� ��� �ֳ���.
	if (gameType == MMATCH_GAMETYPE_DUEL)
	{
		MMatchRuleDuel* pDuel = (MMatchRuleDuel*)pRule;		// RTTI �ȽἭ dynamic cast�� �н�.. ����ó���� ¥������ -,.-
		if (uidChar != pDuel->uidChampion && uidChar != pDuel->uidChallenger)
		{
			OnDuelSetObserver(uidChar);
			return;
		}
	}

	pObj->ResetCustomItemUseCount();
	pObj->SetAlive(true);

	MCommand* pCmd = CreateCommand(MC_MATCH_GAME_RESPONSE_SPAWN, MUID(0,0));
	pCmd->AddParameter(new MCmdParamUID(uidChar));
	pCmd->AddParameter(new MCmdParamShortVector(pos.x, pos.y, pos.z));
	pCmd->AddParameter(new MCmdParamShortVector(DirElementToShort(dir.x), DirElementToShort(dir.y), DirElementToShort(dir.z)));
	RouteToBattle(pObj->GetStageUID(), pCmd);
}

void MMatchServer::OnGameRequestTimeSync(const MUID& uidComm, unsigned long nLocalTimeStamp)
{
	MMatchObject* pObj = GetPlayerByCommUID(uidComm);
	if (pObj == NULL) return;

	MMatchTimeSyncInfo* pSync = pObj->GetSyncInfo();
	pSync->Update(GetGlobalClockCount());

	MCommand* pCmd = CreateCommand(MC_MATCH_GAME_RESPONSE_TIMESYNC, MUID(0,0));
	pCmd->AddParameter(new MCmdParamUInt(nLocalTimeStamp));
	pCmd->AddParameter(new MCmdParamUInt(GetGlobalClockCount()));
	RouteToListener(pObj, pCmd);
}

void MMatchServer::OnGameReportTimeSync(const MUID& uidComm, unsigned long nLocalTimeStamp, unsigned int nDataChecksum)
{
	MMatchObject* pObj = GetPlayerByCommUID(uidComm);
	if (pObj == NULL) return;

	pObj->UpdateTickLastPacketRecved();	// Last Packet Timestamp

	if (pObj->GetEnterBattle() == false)
		return;

	//// SpeedHack Test ////
	MMatchTimeSyncInfo* pSync = pObj->GetSyncInfo();
	int nSyncDiff = nLocalTimeStamp - pSync->GetLastSyncClock();
	float fError = static_cast<float>(nSyncDiff) / static_cast<float>(MATCH_CYCLE_CHECK_SPEEDHACK);
	if (fError > 2.f) {	
		pSync->AddFoulCount();
		if (pSync->GetFoulCount() >= 3) {	// 3���� ���ǵ��� ���� - 3���ƿ�

			#ifndef _DEBUG		// ������Ҷ��� ��������
				NotifyMessage(pObj->GetUID(), MATCHNOTIFY_GAME_SPEEDHACK);
				StageLeave(pObj->GetUID());//, pObj->GetStageUID());
				Disconnect(pObj->GetUID());
			#endif
			mlog("SPEEDHACK : User='%s', SyncRatio=%f (TimeDiff=%d) \n", 
				pObj->GetName(), fError, nSyncDiff);
			pSync->ResetFoulCount();
		}
	} else {
		pSync->ResetFoulCount();
	}
	pSync->Update(GetGlobalClockCount());

	//// MemoryHack Test ////
	if (nDataChecksum > 0) {	// ������ Client MemoryChecksum �𸣹Ƿ� �ϴ� Ŭ���̾�Ʈ�� �Ű��ϴ��ǹ̷� ����Ѵ�.
		NotifyMessage(pObj->GetUID(), MATCHNOTIFY_GAME_MEMORYHACK);
		StageLeave(pObj->GetUID());//, pObj->GetStageUID());
		Disconnect(pObj->GetUID());
		mlog("MEMORYHACK : User='%s', MemoryChecksum=%u \n", pObj->GetName(), nDataChecksum);
	}
}

void MMatchServer::OnUpdateFinishedRound(const MUID& uidStage, const MUID& uidChar, 
						   void* pPeerInfo, void* pKillInfo)
{

}

void MMatchServer::OnRequestForcedEntry(const MUID& uidStage, const MUID& uidChar)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;
	MMatchObject* pObj = GetObject(uidChar);	
	if (pObj == NULL) return;

	pObj->SetForcedEntry(true);

	RouteResponseToListener(pObj, MC_MATCH_STAGE_RESPONSE_FORCED_ENTRY, MOK);
}

void MMatchServer::OnRequestSuicide(const MUID& uidPlayer)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;
	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	pStage->ReserveSuicide( uidPlayer, MGetMatchServer()->GetGlobalClockCount() );

	// OnGameKill(uidPlayer, uidPlayer);

	//MCommand* pNew = CreateCommand(MC_MATCH_RESPONSE_SUICIDE, MUID(0,0));
	//int nResult = MOK;
	//pNew->AddParameter(new MCommandParameterInt(nResult));
	//pNew->AddParameter(new MCommandParameterUID(uidPlayer));
	//RouteToBattle(pObj->GetStageUID(), pNew);
}

void MMatchServer::OnRequestObtainWorldItem(const MUID& uidPlayer, const int nItemUID)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;
	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	pStage->ObtainWorldItem(pObj, nItemUID);
}

void MMatchServer::OnRequestSpawnWorldItem(const MUID& uidPlayer, const int nItemID, const float x, const float y, const float z, float fDropDelayTime)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;
	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	if( !pObj->IsHaveCustomItem() )
		return;

	/*
	�����ϰ� �ִ� ���� �� �� �����Ƿ� ������ ������ �ִ� ����� �� �ִ� ������ ������
	�� �ѵ� �ȿ��� ����� �� �ֵ��� �Ѵ�.
	*/
	if( pObj->IncreaseCustomItemUseCount() )
	{
		pStage->RequestSpawnWorldItem(pObj, nItemID, x, y, z, fDropDelayTime);
	}
}

void MMatchServer::OnNotifyThrowTrapItem(const MUID& uidPlayer, const int nItemID)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;
	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	if (!pObj->IsEquipCustomItem(nItemID))
		return;

	pStage->OnNotifyThrowTrapItem(uidPlayer, nItemID);
}

void MMatchServer::OnNotifyActivatedTrapItem(const MUID& uidPlayer, const int nItemID, const MVector3& pos)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;
	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	pStage->OnNotifyActivatedTrapItem(uidPlayer, nItemID, pos);
}

float MMatchServer::GetDuelVictoryMultiflier(int nVictorty)
{
	return 1.0f;
}

float MMatchServer::GetDuelPlayersMultiflier(int nPlayerCount)
{
	return 1.0f;
}

void MMatchServer::CalcExpOnGameKill(MMatchStage* pStage, MMatchObject* pAttacker, MMatchObject* pVictim, 
					   int* poutAttackerExp, int* poutVictimExp)
{
	bool bSuicide = false;		// �ڻ�
	if (pAttacker == pVictim) bSuicide = true;		

	MMATCH_GAMETYPE nGameType = pStage->GetStageSetting()->GetGameType();
	float fGameExpRatio = MGetGameTypeMgr()->GetInfo(nGameType)->fGameExpRatio;

	// ����Ÿ���� Training�̸� �ٷ� 0����
	if (nGameType == MMATCH_GAMETYPE_TRAINING)
	{
		*poutAttackerExp = 0;
		*poutVictimExp = 0;
		return;
	}
	// ����Ÿ���� ����Ŀ�� ���
	else if (nGameType == MMATCH_GAMETYPE_BERSERKER)
	{
		MMatchRuleBerserker* pRuleBerserker = (MMatchRuleBerserker*)pStage->GetRule();

		if (pRuleBerserker->GetBerserker() == pAttacker->GetUID())
		{
			if (pAttacker != pVictim)
			{
				// ����Ŀ�� ����ġ�� 80%�� ȹ���Ѵ�.
				fGameExpRatio = fGameExpRatio * 0.8f;
			}
			else
			{
				// ����Ŀ�� �ڻ� �Ǵ� �ǰ� �پ� �״°�� �ս� ����ġ�� ������ �Ѵ�.
				fGameExpRatio = 0.0f;
			}
		}
	}
	else if (nGameType == MMATCH_GAMETYPE_DUEL)
	{
		MMatchRuleDuel* pRuleDuel = (MMatchRuleDuel*)pStage->GetRule();
		if (pVictim->GetUID() == pRuleDuel->uidChallenger)
		{
			fGameExpRatio *= GetDuelVictoryMultiflier(pRuleDuel->GetVictory());
		}
		else
		{
			fGameExpRatio *= GetDuelVictoryMultiflier(pRuleDuel->GetVictory()) * GetDuelPlayersMultiflier(pStage->GetPlayers());

		}
//		if (pRuleDuel->GetVictory() <= 1)
//		{
//			fGameExpRatio = fGameExpRatio * GetDuelPlayersMultiflier(pStage->GetPlayers()) * GetDuelVictoryMultiflier()
//		}
	}
	else if (nGameType == MMATCH_GAMETYPE_CTF)
	{
			MMatchRuleTeamCTF* pRuleCTF = (MMatchRuleTeamCTF*)pStage->GetRule();
			if (pAttacker != pVictim)
			{
				// ����Ŀ�� ����ġ�� 80%�� ȹ���Ѵ�.
				if(!(pAttacker->GetTeam() == pVictim->GetTeam()) && (pVictim->GetUID() == pRuleCTF->GetBlueCarrier() || pVictim->GetUID() == pRuleCTF->GetRedCarrier()))
				fGameExpRatio = fGameExpRatio * 2;
			}
			else
			{
				// ����Ŀ�� �ڻ� �Ǵ� �ǰ� �پ� �״°�� �ս� ����ġ�� ������ �Ѵ�.
				fGameExpRatio = 0.0f;
			}
	}

	// ��, ����Ÿ�Կ� ���� ����ġ ���� ����
	int nMapIndex = pStage->GetStageSetting()->GetMapIndex();
	if ((nMapIndex >=0) && (nMapIndex < MMATCH_MAP_COUNT))
	{
		float fMapExpRatio = MGetMapDescMgr()->GetExpRatio(nMapIndex);
		fGameExpRatio = fGameExpRatio * fMapExpRatio;
	}

	int nAttackerLevel = pAttacker->GetCharInfo()->m_nLevel;
	int nVictimLevel = pVictim->GetCharInfo()->m_nLevel;

	// ����ġ ���
	int nAttackerExp = (int)(MMatchFormula::GetGettingExp(nAttackerLevel, nVictimLevel) * fGameExpRatio);
	int nVictimExp = (int)(MMatchFormula::CalcPanaltyEXP(nAttackerLevel, nVictimLevel) * fGameExpRatio);


	// Ŭ������ ���� ȹ�� ����ġ�� 1.5��, �սǰ���ġ ����
	#ifndef _QUESTCLAN
	if ((MGetServerConfig()->GetServerMode() == MSM_CLAN) && (pStage->GetStageType() == MST_LADDER))
	#else
		if (pStage->GetStageType() == MST_LADDER)
	#endif
		{
		nAttackerExp = (int)((float)nAttackerExp * 1.5f);
		nVictimExp = 0;
		}

	// ����ä��, �ʰ���ä���� ��쿡�� ��ġ�ٿ� ����(�ڻ�����)
	MMatchChannel* pOwnerChannel = FindChannel(pStage->GetOwnerChannel());
	if ((pOwnerChannel) && (!bSuicide))
	{
		if ((pOwnerChannel->GetRuleType() == MCHANNEL_RULE_MASTERY) || 
			(pOwnerChannel->GetRuleType() == MCHANNEL_RULE_ELITE) ||
			(pOwnerChannel->GetRuleType() == MCHANNEL_RULE_CHAMPION))
		{
			nVictimExp=0;
		}
	}

	// ��������� ���, �������� ��� ����ġ �ι�
	if ((pVictim->GetAccountInfo()->m_nUGrade == MMUG_ADMIN) || 
		(pVictim->GetAccountInfo()->m_nUGrade == MMUG_DEVELOPER))
	{
		nAttackerExp = nAttackerExp * 2;
	}
	// ���λ���� ���, �������� ��� ��ġ�ٿ� ����
	if ((!bSuicide) &&
		((pAttacker->GetAccountInfo()->m_nUGrade == MMUG_ADMIN) || 
		(pAttacker->GetAccountInfo()->m_nUGrade == MMUG_DEVELOPER)))
	{
		nVictimExp = 0;
	}

	// �ڻ��� ��� ����ġ �ս��� �ι�
	if (bSuicide) 
	{
		nVictimExp = (int)(MMatchFormula::GetSuicidePanaltyEXP(nVictimLevel) * fGameExpRatio);
		nAttackerExp = 0;
	}

	// ��ų�ΰ�� ����ġ ����
	if ((pStage->GetStageSetting()->IsTeamPlay()) && (pAttacker->GetTeam() == pVictim->GetTeam()))
	{
		nAttackerExp = 0;
	}


	// ������ ��� ����ġ ���
	if (pStage->IsApplyTeamBonus())
	{
		int nTeamBonus = 0;
		if (pStage->GetRule() != NULL)
		{
			int nNewAttackerExp = nAttackerExp;
			pStage->GetRule()->CalcTeamBonus(pAttacker, pVictim, nAttackerExp, &nNewAttackerExp, &nTeamBonus);
			nAttackerExp = nNewAttackerExp;
		}

		// �� ����ġ ����
		pStage->AddTeamBonus(nTeamBonus, MMatchTeam(pAttacker->GetTeam()));
	}

	// xp ���ʽ� ����(�ݸ��� PC��, ����ġ ����)
	int nAttackerExpBonus = 0;
	if (nAttackerExp != 0)
	{
		//const float ta = float(atoi("15")) / 100.0f;
		//mlog( "test float : %f\n", ta * 100.0f );

		//MMatchItemBonusType nBonusType			= GetStageBonusType(pStage->GetStageSetting());
		//const double		dAttackerExp		= static_cast< double >( nAttackerExp );
		//const double		dXPBonusRatio		= static_cast< double >( MMatchFormula::CalcXPBonusRatio(pAttacker, nBonusType) );
		//const double		dAttackerExpBouns	= dAttackerExp * dXPBonusRatio;
		//const double		dSumAttackerExp		= dAttackerExp + dAttackerExpBouns;
		//
		//
		//nAttackerExpBonus = static_cast< int >( dAttackerExpBouns + 0.00001); 

		MMatchItemBonusType nBonusType = GetStageBonusType(pStage->GetStageSetting());
		const float fAttackerExpBonusRatio = MMatchFormula::CalcXPBonusRatio(pAttacker, nBonusType);
		 //�ε��Ҽ��� ���������� ��꿡 ������ ���� �ʴ� �������� ������ ���ش�.
		// ���� �̺κп��� �ٽ� ������ �߻��Ѵٸ� ������ �ƴ� ���̽����� ���� �۾��� �� ��� �Ѵ�.
		 nAttackerExpBonus = (int)(nAttackerExp * (fAttackerExpBonusRatio + 0.00001f));
	}

	*poutAttackerExp = nAttackerExp + nAttackerExpBonus;

	*poutVictimExp = nVictimExp;
}


const int MMatchServer::CalcBPonGameKill( MMatchStage* pStage, MMatchObject* pAttacker, const int nAttackerLevel, const int nVictimLevel )
{
	if( (0 == pStage) || (0 == pAttacker) ) 
		return -1;

	const int	nAddedBP		= static_cast< int >( MMatchFormula::GetGettingBounty(nAttackerLevel, nVictimLevel) );
	const float fBPBonusRatio	= MMatchFormula::CalcBPBounsRatio( pAttacker, GetStageBonusType(pStage->GetStageSetting()) );
	const int	nBPBonus		= static_cast< int >( nAddedBP * fBPBonusRatio );

	return nAddedBP + nBPBonus;
}




void MMatchServer::ProcessPlayerXPBP(MMatchStage* pStage, MMatchObject* pPlayer, int nAddedXP, int nAddedBP)
{
	if (pStage == NULL) return;
	if (!IsEnabledObject(pPlayer)) return;

	/*
		����ġ ���
		ĳ���Ϳ� ����ġ ����
		���� ���
		DBĳ�� ������Ʈ
		������,�ٿ� �޼��� ����
	*/

	MUID uidStage = pPlayer->GetStageUID();
	int nPlayerLevel = pPlayer->GetCharInfo()->m_nLevel;

	// ĳ���� XP ������Ʈ
	pPlayer->GetCharInfo()->IncXP(nAddedXP);

	// ���� ���
	int nNewPlayerLevel = -1;
	if ((pPlayer->GetCharInfo()->m_nLevel < MAX_LEVEL) &&
		(pPlayer->GetCharInfo()->m_nXP >= MMatchFormula::GetNeedExp(nPlayerLevel)))
	{
		nNewPlayerLevel = MMatchFormula::GetLevelFromExp(pPlayer->GetCharInfo()->m_nXP);
		if (nNewPlayerLevel != pPlayer->GetCharInfo()->m_nLevel) pPlayer->GetCharInfo()->m_nLevel = nNewPlayerLevel;
	}

	// �ٿ�Ƽ �߰����ش�
	pPlayer->GetCharInfo()->IncBP(nAddedBP);


	// DB ĳ�� ������Ʈ
	if (pPlayer->GetCharInfo()->GetDBCachingData()->IsRequestUpdate()) {
		UpdateCharDBCachingData(pPlayer);		///< XP, BP, KillCount, DeathCount ĳ�� ������Ʈ
	}

	// ���� ������ �ٲ�� ���� �������Ѵ�.
	if ((nNewPlayerLevel >= 0) && (nNewPlayerLevel != nPlayerLevel))
	{
		// ������ �ٲ�� �ٷ� ĳ�� ������Ʈ�Ѵ�
		UpdateCharDBCachingData(pPlayer);

		pPlayer->GetCharInfo()->m_nLevel = nNewPlayerLevel;
		if (!m_MatchDBMgr.UpdateCharLevel(pPlayer->GetCharInfo()->m_nCID, 
										  nNewPlayerLevel, 
										  pPlayer->GetCharInfo()->m_nBP,
										  pPlayer->GetCharInfo()->m_nTotalKillCount, 
										  pPlayer->GetCharInfo()->m_nTotalDeathCount,
										  pPlayer->GetCharInfo()->m_nTotalPlayTimeSec,
										  true))
		{
			mlog("DB UpdateCharLevel Error : %s\n", pPlayer->GetCharInfo()->m_szName);
		}
	}

	// ������, ���� �ٿ� �޼��� ������
	if (nNewPlayerLevel > 0)
	{
		if (nNewPlayerLevel > nPlayerLevel)
		{
			MCommand* pCmd = CreateCommand(MC_MATCH_GAME_LEVEL_UP, MUID(0,0));
			pCmd->AddParameter(new MCommandParameterUID(pPlayer->GetUID()));
			pCmd->AddParameter(new MCommandParameterInt(nNewPlayerLevel));
			RouteToBattle(uidStage, pCmd);	
		}
		else if (nNewPlayerLevel < nPlayerLevel)
		{
			MCommand* pCmd = CreateCommand(MC_MATCH_GAME_LEVEL_DOWN, MUID(0,0));
			pCmd->AddParameter(new MCommandParameterUID(pPlayer->GetUID()));
			pCmd->AddParameter(new MCommandParameterInt(nNewPlayerLevel));
			RouteToBattle(uidStage, pCmd);	
		}
	}
}

// �� ���ʽ� ����
void MMatchServer::ApplyObjectTeamBonus(MMatchObject* pObject, int nAddedExp)
{
	if (!IsEnabledObject(pObject)) return;
	if (nAddedExp <= 0)
	{
		_ASSERT(0);
		return;
	}
	
	bool bIsLevelUp = false;

	// ���ʽ� ����
	if (nAddedExp != 0)
	{
		int nExpBonus = (int)(nAddedExp * MMatchFormula::CalcXPBonusRatio(pObject, MIBT_TEAM));
		nAddedExp += nExpBonus;
	}




	// ĳ���� XP ������Ʈ
	pObject->GetCharInfo()->IncXP(nAddedExp);

	// ���� ���
	int nNewLevel = -1;
	int nCurrLevel = pObject->GetCharInfo()->m_nLevel;

	if (nNewLevel > nCurrLevel) bIsLevelUp = true;

	if ((pObject->GetCharInfo()->m_nLevel < MAX_LEVEL) &&
		(pObject->GetCharInfo()->m_nXP >= MMatchFormula::GetNeedExp(nCurrLevel)))
	{
		nNewLevel = MMatchFormula::GetLevelFromExp(pObject->GetCharInfo()->m_nXP);
		if (nNewLevel != nCurrLevel) pObject->GetCharInfo()->m_nLevel = nNewLevel;
	}

	// DB ĳ�� ������Ʈ
	if (pObject->GetCharInfo()->GetDBCachingData()->IsRequestUpdate())
	{
		UpdateCharDBCachingData(pObject);
	}

	// ���� ������ �ٲ�� �ٷ� �������Ѵ�.
	if ((nNewLevel >= 0) && (nNewLevel != nCurrLevel))
	{
		// ������ �ٲ�� �ٷ� ĳ�� ������Ʈ�Ѵ�
		UpdateCharDBCachingData(pObject);

		pObject->GetCharInfo()->m_nLevel = nNewLevel;
		nCurrLevel = nNewLevel;

		if (!m_MatchDBMgr.UpdateCharLevel(pObject->GetCharInfo()->m_nCID, 
			                              nNewLevel,
										  pObject->GetCharInfo()->m_nBP,
										  pObject->GetCharInfo()->m_nTotalKillCount,
										  pObject->GetCharInfo()->m_nTotalDeathCount,
										  pObject->GetCharInfo()->m_nTotalPlayTimeSec,
										  bIsLevelUp
										  ))
		{
			mlog("DB UpdateCharLevel Error : %s\n", pObject->GetCharInfo()->m_szName);
		}
	}


	MUID uidStage = pObject->GetStageUID();

	unsigned long int nExpArg;
	unsigned long int nChrExp;
	int nPercent;

	nChrExp = pObject->GetCharInfo()->m_nXP;
	nPercent = MMatchFormula::GetLevelPercent(nChrExp, nCurrLevel);
	// ���� 2����Ʈ�� ����ġ, ���� 2����Ʈ�� ����ġ�� �ۼ�Ʈ�̴�.
	nExpArg = MakeExpTransData(nAddedExp, nPercent);


	MCommand* pCmd = CreateCommand(MC_MATCH_GAME_TEAMBONUS, MUID(0,0));
	pCmd->AddParameter(new MCommandParameterUID(pObject->GetUID()));
	pCmd->AddParameter(new MCommandParameterUInt(nExpArg));
	RouteToBattle(uidStage, pCmd);	


	// ������ �޼��� ������
	if ((nNewLevel >= 0) && (nNewLevel > nCurrLevel))
	{
		MCommand* pCmd = CreateCommand(MC_MATCH_GAME_LEVEL_UP, MUID(0,0));
		pCmd->AddParameter(new MCommandParameterUID(pObject->GetUID()));
		pCmd->AddParameter(new MCommandParameterInt(nNewLevel));
		RouteToBattle(uidStage, pCmd);	
	}
}

// �÷��� �� ĳ���� ���� ������Ʈ
void MMatchServer::ProcessCharPlayInfo(MMatchObject* pPlayer)
{
	if (!IsEnabledObject(pPlayer)) return;

	/*
	���Ҷ����� ĳ���� ������ ������Ʈ ����
	����ġ ���
	ĳ���Ϳ� ����ġ ����
	���� ���
	������,�ٿ� �޼��� ����
	�ٿ�Ƽ �߰����ش�
	���ӽð�, ���� ����ð�, �÷��� �ð�
	*/

	MUID uidStage = pPlayer->GetStageUID();
	int nPlayerLevel = pPlayer->GetCharInfo()->m_nLevel;

	// ���� ���
	int nNewPlayerLevel = -1;
	if ((pPlayer->GetCharInfo()->m_nLevel < MAX_LEVEL) &&
		(pPlayer->GetCharInfo()->m_nXP >= MMatchFormula::GetNeedExp(nPlayerLevel)))
	{
		nNewPlayerLevel = MMatchFormula::GetLevelFromExp(pPlayer->GetCharInfo()->m_nXP);
		if (nNewPlayerLevel != pPlayer->GetCharInfo()->m_nLevel) pPlayer->GetCharInfo()->m_nLevel = nNewPlayerLevel;
	}
	// ���� ������ �ٲ�� ���� �������Ѵ�.
	if ((nNewPlayerLevel >= 0) && (nNewPlayerLevel != nPlayerLevel))
		pPlayer->GetCharInfo()->m_nLevel = nNewPlayerLevel;

	// ���ӽð�, ���� ����ð�, �÷��� �ð�
	unsigned long int nNowTime = MMatchServer::GetInstance()->GetGlobalClockCount();
	unsigned long int nBattlePlayingTimeSec = 0;
	if(pPlayer->GetCharInfo()->m_nBattleStartTime != 0)
	{
		nBattlePlayingTimeSec = MGetTimeDistance(pPlayer->GetCharInfo()->m_nBattleStartTime, nNowTime) / 1000;	// ��Ʋ�� ������ �ð�
		
		/*
		// �ƹ� ó���� ���� �ʴµ�, �� �α״� ����°ǰ���? �ϴ� �ּ� ó���մϴ�. - carrot318
		if(nBattlePlayingTimeSec > 60*60)
		{// �̻������� ���� ���õŸ� �α׸� �����.
			CTime theTime = CTime::GetCurrentTime();
			CString szTime = theTime.Format( "[%c] " );

			// ���� ���
			char buf[64]={0,};
			MMatchStage* pStage = FindStage(uidStage);

			if( pStage != NULL )
			{
				switch((int)pStage->GetStageSetting()->GetGameType())
				{
				case MMATCH_GAMETYPE_DEATHMATCH_SOLO:	{sprintf(buf, "DEATHMATCH_SOLO");	} break;		///< ���� ������ġ
				case MMATCH_GAMETYPE_DEATHMATCH_TEAM:	{sprintf(buf, "DEATHMATCH_TEAM");	} break;		///< �� ������ġ
				case MMATCH_GAMETYPE_GLADIATOR_SOLO:	{sprintf(buf, "GLADIATOR_SOLO");	} break;		///< ���� �۷�������
				case MMATCH_GAMETYPE_GLADIATOR_TEAM:	{sprintf(buf, "GLADIATOR_TEAM");	} break;		///< �� �۷�������
				case MMATCH_GAMETYPE_ASSASSINATE:		{sprintf(buf, "ASSASSINATE");		} break;		///< ������
				case MMATCH_GAMETYPE_TRAINING:			{sprintf(buf, "TRAINING");			} break;		///< ����

				case MMATCH_GAMETYPE_SURVIVAL:			{sprintf(buf, "SURVIVAL");			} break;		///< �����̹�
				case MMATCH_GAMETYPE_QUEST:				{sprintf(buf, "QUEST");				} break;		///< ����Ʈ

				case MMATCH_GAMETYPE_BERSERKER:			{sprintf(buf, "BERSERKER");			} break;		
				case MMATCH_GAMETYPE_DEATHMATCH_TEAM2:	{sprintf(buf, "DEATHMATCH_TEAM2");	} break;		
				case MMATCH_GAMETYPE_DUEL:				{sprintf(buf, "DUEL");				} break;	
				default:								{sprintf(buf, "don't know");		} break;
				}
				mlog("%s BattlePlayT Error GameMode:%s, CID:%d, Name:%s, ServerCurrT:%u, BattleStartT:%u, PlayT:%d, PlayerConnectT:%u \n"
					, szTime, buf, pPlayer->GetCharInfo()->m_nCID, pPlayer->GetCharInfo()->m_szName, nNowTime, pPlayer->GetCharInfo()->m_nBattleStartTime, nBattlePlayingTimeSec, pPlayer->GetCharInfo()->m_nConnTime);
			}
		}
		*/
		//pPlayer->GetCharInfo()->m_nBattleStartTime = 0;
	}
	unsigned long int nLoginTotalTimeSec = MGetTimeDistance(pPlayer->GetCharInfo()->m_nConnTime, nNowTime) / 1000;	// ������ ������ �ð�

	// �̻������� ����ġ�� ȹ�������� �α׸� �����ش�.
	// �ƹ� ó���� ���� �ʴµ�, �� �α״� ����°ǰ���? �ϴ� �ּ� ó���մϴ�. - carrot318
	/*
	long int nBattleEXPGained = pPlayer->GetCharInfo()->m_nXP - pPlayer->GetCharInfo()->m_nBattleStartXP;
	if(nBattleEXPGained < -150000 || 150000 < nBattleEXPGained)
	{
		CTime theTime = CTime::GetCurrentTime();
		CString szTime = theTime.Format( "[%c] " );
		mlog("%s BattleXPGained Error CID:%d, Name:%s, StartXP:%d, EXPGained:%d \n", szTime, pPlayer->GetCharInfo()->m_nCID, pPlayer->GetCharInfo()->m_szName, pPlayer->GetCharInfo()->m_nBattleStartXP, nBattleEXPGained);
	}
	*/

#ifdef LOCALE_NHNUSA
	if (!m_MatchDBMgr.UpdateCharPlayInfo(pPlayer->GetAccountInfo()->m_nAID
										, pPlayer->GetCharInfo()->m_nCID
										, pPlayer->GetCharInfo()->m_nXP
										, pPlayer->GetCharInfo()->m_nLevel
										, nBattlePlayingTimeSec										// ��Ʋ �ð�
										, nLoginTotalTimeSec										// �Ѱ����� ������ �ð�
										, pPlayer->GetCharInfo()->m_nTotalKillCount
										, pPlayer->GetCharInfo()->m_nTotalDeathCount
										, pPlayer->GetCharInfo()->m_nBP
										, false))
	{
		mlog("DB UpdateCharPlayInfo Error : %s\n", pPlayer->GetCharInfo()->m_szName);
	}
#endif

}

void MMatchServer::PostGameDeadOnGameKill(MUID& uidStage, MMatchObject* pAttacker, MMatchObject* pVictim,
									int nAddedAttackerExp, int nSubedVictimExp)
{
	unsigned long int nAttackerArg = 0;
	unsigned long int nVictimArg =0;

	int nRealAttackerLevel = pAttacker->GetCharInfo()->m_nLevel;
	int nRealVictimLevel = pVictim->GetCharInfo()->m_nLevel;

	unsigned long int nChrExp;
	int nPercent;

	nChrExp = pAttacker->GetCharInfo()->m_nXP;
	nPercent = MMatchFormula::GetLevelPercent(nChrExp, nRealAttackerLevel);
	nAttackerArg = MakeExpTransData(nAddedAttackerExp, nPercent);

	nChrExp = pVictim->GetCharInfo()->m_nXP;
	nPercent = MMatchFormula::GetLevelPercent(nChrExp, nRealVictimLevel);
	nVictimArg = MakeExpTransData(nSubedVictimExp, nPercent);

	MCommand* pCmd = CreateCommand(MC_MATCH_GAME_DEAD, MUID(0,0));
	pCmd->AddParameter(new MCommandParameterUID(pAttacker->GetUID()));
	pCmd->AddParameter(new MCommandParameterUInt(nAttackerArg));
	pCmd->AddParameter(new MCommandParameterUID(pVictim->GetUID()));
	pCmd->AddParameter(new MCommandParameterUInt(nVictimArg));
	RouteToBattle(uidStage, pCmd);	
}

void MMatchServer::StageList(const MUID& uidPlayer, int nStageStartIndex, bool bCacheUpdate)
{
	MMatchObject* pChar = GetObject(uidPlayer);
	if (pChar == NULL) return;
	MMatchChannel* pChannel = FindChannel(pChar->GetChannelUID());
	if (pChannel == NULL) return;

	// Ŭ�������ε� Ŭ��ä���� ��쿡�� �� ����Ʈ��� ����� Ŭ�� ����Ʈ�� ������.
	#ifndef _QUESTCLAN
	if ((MGetServerConfig()->GetServerMode() == MSM_CLAN) && (pChannel->GetChannelType() == MCHANNEL_TYPE_CLAN))
	#else
		if (pChannel->GetChannelType() == MCHANNEL_TYPE_CLAN)
	#endif
		{
		StandbyClanList(uidPlayer, nStageStartIndex, bCacheUpdate);
		return;
	}


	MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_LIST), MUID(0,0), m_This);

	int nPrevStageCount = -1, nNextStageCount = -1;
	int nNextStageIndex = pChannel->GetMaxPlayers()-1;


	// 2008.09.16 
	int nRealStageStartIndex = nStageStartIndex;
	int nStageCount = 0;
	for(int i = 0; i < pChannel->GetMaxPlayers(); i++)
	{
		// ���� ��������� ó�����Ѵ�
		if (pChannel->IsEmptyStage(i)) continue;
		// ���� ������ ó��
		if(nStageCount < nStageStartIndex) // �տ� �ǿ� ó���� ��� < ���� �ǿ��� ������ �� Index
			nStageCount++;
		else
		{
			nRealStageStartIndex = i;
			break;
		}
	}

	int nRealStageCount = 0;
	for (int i = /*nStageStartIndex*/nRealStageStartIndex; i < pChannel->GetMaxPlayers(); i++)
	{
		if (pChannel->IsEmptyStage(i)) continue;

		MMatchStage* pStage = pChannel->GetStage(i);
		if ((pStage == NULL) || (pStage->GetState() == STAGE_STATE_CLOSE)) continue;

		nRealStageCount++;
		if (nRealStageCount >= TRANS_STAGELIST_NODE_COUNT) 
		{
			nNextStageIndex = i;
			break;
		}
	}

	if (!bCacheUpdate)
	{
		nPrevStageCount = pChannel->GetPrevStageCount(nStageStartIndex);
		nNextStageCount = pChannel->GetNextStageCount(nNextStageIndex);
	}

	pNew->AddParameter(new MCommandParameterChar((char)nPrevStageCount));
	pNew->AddParameter(new MCommandParameterChar((char)nNextStageCount));


	void* pStageArray = MMakeBlobArray(sizeof(MTD_StageListNode), nRealStageCount);
	int nArrayIndex=0;

	for (int i = /*nStageStartIndex*/nRealStageStartIndex; i < pChannel->GetMaxPlayers(); i++)
	{
		if (pChannel->IsEmptyStage(i)) continue;
		MMatchStage* pStage = pChannel->GetStage(i);
		if ((pStage == NULL) || (pStage->GetState() == STAGE_STATE_CLOSE)) continue;
		
		if( pStage->GetState() < STAGE_STATE_STANDBY || pStage->GetState() > STAGE_STATE_COUNT )
		{
			/* ũ������ ���� ����ڵ� �߰�. ���߿� pChannel->m_pStages �迭�� �ƴ� ���̳� �ٸ��ɷ� �����丵�ʿ�*/
			LOG(LOG_FILE, "there is unavailable stages in %s channel. No:%d \n", pChannel->GetName(), i);
			continue;
		}


		if (nArrayIndex >= nRealStageCount) break;

		MTD_StageListNode* pNode = (MTD_StageListNode*)MGetBlobArrayElement(pStageArray, nArrayIndex++);
		pNode->uidStage = pStage->GetUID();
		strcpy(pNode->szStageName, pStage->GetName());
		pNode->nNo = (unsigned char)(pStage->GetIndex() + 1);	// ����ڿ��� �����ִ� �ε����� 1���� �����Ѵ�
		pNode->nPlayers = (char)pStage->GetPlayers();
		pNode->nMaxPlayers = pStage->GetStageSetting()->GetMaxPlayers();
		pNode->nState = pStage->GetState();
		pNode->nGameType = pStage->GetStageSetting()->GetGameType();
		
		// �����̸� �κ� �渮��Ʈ ��ʸ� �����̸����� �������ش�.
		if(pStage->IsRelayMap()) pNode->nMapIndex = MMATCH_MAP_RELAYMAP;
		else		 			 pNode->nMapIndex = pStage->GetStageSetting()->GetMapIndex();
		
		pNode->nSettingFlag = 0;
		// ����
		if (pStage->GetStageSetting()->GetForcedEntry())
		{
			pNode->nSettingFlag |= MSTAGENODE_FLAG_FORCEDENTRY_ENABLED;
		}
		// ��й�
		if (pStage->IsPrivate())
		{
			pNode->nSettingFlag |= MSTAGENODE_FLAG_PRIVATE;
		}
		// ��������
		pNode->nLimitLevel = pStage->GetStageSetting()->GetLimitLevel();
		pNode->nMasterLevel = 0;

		if (pNode->nLimitLevel != 0)
		{
			pNode->nSettingFlag |= MSTAGENODE_FLAG_LIMITLEVEL;

			;
			MMatchObject* pMaster = GetObject(pStage->GetMasterUID());
			if (pMaster)
			{
				if (pMaster->GetCharInfo())
				{
					pNode->nMasterLevel = pMaster->GetCharInfo()->m_nLevel;
				}
			}
		}
	}

	pNew->AddParameter(new MCommandParameterBlob(pStageArray, MGetBlobArraySize(pStageArray)));
	MEraseBlobArray(pStageArray);

	RouteToListener(pChar, pNew);	
}


void MMatchServer::OnStageRequestStageList(const MUID& uidPlayer, const MUID& uidChannel, const int nStageCursor)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;
	MMatchChannel* pChannel = FindChannel(uidChannel);
	if (pChannel == NULL) return;

	pObj->SetStageCursor(nStageCursor);
	StageList(pObj->GetUID(), nStageCursor, false);
}


void MMatchServer::OnRequestQuickJoin(const MUID& uidPlayer, void* pQuickJoinBlob)
{
	MTD_QuickJoinParam* pNode = (MTD_QuickJoinParam*)MGetBlobArrayElement(pQuickJoinBlob, 0);
	ResponseQuickJoin(uidPlayer, pNode);
}

void MMatchServer::ResponseQuickJoin(const MUID& uidPlayer, MTD_QuickJoinParam* pQuickJoinParam)
{
	if (pQuickJoinParam == NULL) return;

	MMatchObject* pObj = GetObject(uidPlayer);
	if (!IsEnabledObject(pObj)) return;
	MMatchChannel* pChannel = FindChannel(pObj->GetChannelUID());
	if (pChannel == NULL) return;

	list<MUID>	recommended_stage_list;
	MUID uidRecommendedStage = MUID(0,0);
	int nQuickJoinResult = MOK;


	for (int i = 0; i < pChannel->GetMaxStages(); i++)
	{
		if (pChannel->IsEmptyStage(i)) continue;
		MMatchStage* pStage = pChannel->GetStage(i);
		if ((pStage == NULL) || (pStage->GetState() == STAGE_STATE_CLOSE)) continue;

		int ret = ValidateStageJoin(pObj->GetUID(), pStage->GetUID());
		if (ret == MOK)
		{
			if (pStage->IsPrivate()) continue;

			int nMapIndex = pStage->GetStageSetting()->GetMapIndex();
			int nGameType = pStage->GetStageSetting()->GetGameType();

			if (!CheckBitSet(pQuickJoinParam->nMapEnum, nMapIndex)) continue;
			if (!CheckBitSet(pQuickJoinParam->nModeEnum, nGameType)) continue;

			//if (((1 << nMapIndex) & (pQuickJoinParam->nMapEnum)) == 0) continue;
			//if (((1 << nGameType) & (pQuickJoinParam->nModeEnum)) == 0) continue;

			recommended_stage_list.push_back(pStage->GetUID());
		}
	}

	if (!recommended_stage_list.empty())
	{
		int nSize=(int)recommended_stage_list.size();
		int nIndex = rand() % nSize;

		int nCnt = 0;
		for (list<MUID>::iterator itor = recommended_stage_list.begin(); itor != recommended_stage_list.end(); ++itor)
		{
			if (nIndex == nCnt)
			{
				uidRecommendedStage = (*itor);
				break;
			}
			nCnt++;
		}
	}
	else
	{
		nQuickJoinResult = MERR_CANNOT_NO_STAGE;
	}

	MCommand* pCmd = CreateCommand(MC_MATCH_STAGE_RESPONSE_QUICKJOIN, MUID(0,0));
	pCmd->AddParameter(new MCommandParameterInt(nQuickJoinResult));
	pCmd->AddParameter(new MCommandParameterUID(uidRecommendedStage));
	RouteToListener(pObj, pCmd);	
}

static int __cdecl _int_sortfunc(const void* a, const void* b)
{
	return *((int*)a) - *((int*)b);
}


int MMatchServer::GetLadderTeamIDFromDB(const int nTeamTableIndex, const int* pnMemberCIDArray, const int nMemberCount)
{
	if ((nMemberCount <= 0) || (nTeamTableIndex != nMemberCount))
	{
		_ASSERT(0);
		return 0;
	}

	// cid ������������ ���� - db�� ���õǾ� ���ִ�. 
	int* pnSortedCIDs = new int[nMemberCount];
	for (int i = 0; i < nMemberCount; i++)
	{
		pnSortedCIDs[i] = pnMemberCIDArray[i];
	}
	qsort(pnSortedCIDs, nMemberCount, sizeof(int), _int_sortfunc);

	int nTID = 0;
	if (pnSortedCIDs[0] != 0)
	{
		if (!m_MatchDBMgr.GetLadderTeamID(nTeamTableIndex, pnSortedCIDs, nMemberCount, &nTID))
		{
			nTID = 0;
		}
	}
	

	delete[] pnSortedCIDs;

	return nTID;
}

void MMatchServer::SaveLadderTeamPointToDB(const int nTeamTableIndex, const int nWinnerTeamID, const int nLoserTeamID, const bool bIsDrawGame)
{
	// ����Ʈ ��� - �׼Ǹ��� ����
	int nWinnerPoint = 0, nLoserPoint = 0, nDrawPoint = 0;

	nLoserPoint = -1;
	switch (nTeamTableIndex)
	{
	case 2:	// 2��2
		{
			nWinnerPoint = 4;
			nDrawPoint = 1;
		}
		break;
	case 3:
		{
			nWinnerPoint = 6;
			nDrawPoint = 1;
		}
		break;
	case 4:
		{
			nWinnerPoint = 10;
			nDrawPoint = 2;
		}
		break;
	}

	if (!m_MatchDBMgr.LadderTeamWinTheGame(nTeamTableIndex, nWinnerTeamID, nLoserTeamID, bIsDrawGame,
		                                   nWinnerPoint, nLoserPoint, nDrawPoint))
	{
		mlog("DB Query(SaveLadderTeamPointToDB) Failed\n");
	}
}


void MMatchServer::OnVoteCallVote(const MUID& uidPlayer, const char* pszDiscuss, const char* pszArg)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	// ��ڰ� ������ǥ�ϸ� ������ ����
	if (IsAdminGrade(pObj)) {
		MMatchStage* pStage = FindStage(pObj->GetStageUID());
		if (pStage)
			pStage->KickBanPlayer(pszArg, false);
		return;
	}

	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	char szMsg[256];
	// ��ڰ� ���� �������̸� ��ǥ �Ұ���
	for (MUIDRefCache::iterator itor = pStage->GetObjBegin(); itor != pStage->GetObjEnd(); itor++) {
		MUID uidObj = (MUID)(*itor).first;
		MMatchObject* pPlayer = (MMatchObject*)GetObject(uidObj);
		if ((pPlayer) && (IsAdminGrade(pPlayer)))
		{
			sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_CANNOT_VOTE);
			Announce(uidPlayer, szMsg);

			return;
		}
	}


	if( pObj->WasCallVote() )
	{
		sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_CANNOT_VOTE);
		Announce(uidPlayer, szMsg);

		return;
	}

	// ��ǥ�� �ߴٴ°� ǥ���س���.
	pObj->SetVoteState( true );

	if (pStage->GetStageType() == MST_LADDER)
	{
		sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_CANNOT_VOTE_LADERGAME);
		Announce(uidPlayer, szMsg);

		return;
	}

	if (pStage->GetRule() && pStage->GetRule()->GetGameType() == MMATCH_GAMETYPE_DUELTOURNAMENT)
	{
		sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_CANNOT_VOTE);
		Announce(uidPlayer, szMsg);

		return;
	}
#ifdef _VOTESETTING
	// �� ������ ��ǥ����� �˻���.
	if( !pStage->GetStageSetting()->bVoteEnabled ) {
		VoteAbort( uidPlayer );
		return;
	}

	// �̹� ���ӿ��� ��ǥ�� �����ߴ��� �˻�.
	if( pStage->WasCallVote() ) {
		VoteAbort( uidPlayer );
		return;
	}
	else {
		pStage->SetVoteState( true );
	}
#endif

	if (pStage->GetVoteMgr()->GetDiscuss())
	{
		sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_VOTE_ALREADY_START);
		Announce(uidPlayer, szMsg);

		return;
	}

	MVoteDiscuss* pDiscuss = MVoteDiscussBuilder::Build(uidPlayer, pStage->GetUID(), pszDiscuss, pszArg);
	if (pDiscuss == NULL) return;

	if (pStage->GetVoteMgr()->CallVote(pDiscuss)) {
		pDiscuss->Vote(uidPlayer, MVOTE_YES);	// ������ ������ ����

		MCommand* pCmd = CreateCommand(MC_MATCH_NOTIFY_CALLVOTE, MUID(0,0));
		pCmd->AddParameter(new MCmdParamStr(pszDiscuss));
		pCmd->AddParameter(new MCmdParamStr(pszArg));
		RouteToStage(pStage->GetUID(), pCmd);
		return;
	}
	else
	{
		sprintf(szMsg, "%s%d", MTOK_ANNOUNCE_PARAMSTR, MERR_VOTE_FAILED);
		Announce(uidPlayer, szMsg);

		return;
	}
}

void MMatchServer::OnVoteYes(const MUID& uidPlayer)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	MVoteDiscuss* pDiscuss = pStage->GetVoteMgr()->GetDiscuss();
    if (pDiscuss == NULL) return;

	pDiscuss->Vote(uidPlayer, MVOTE_YES);
}

void MMatchServer::OnVoteNo(const MUID& uidPlayer)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;

	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	MVoteDiscuss* pDiscuss = pStage->GetVoteMgr()->GetDiscuss();
    if (pDiscuss == NULL) return;

	pDiscuss->Vote(uidPlayer, MVOTE_NO);
}

void MMatchServer::VoteAbort( const MUID& uidPlayer )
{
#ifndef MERR_CANNOT_VOTE
#define MERR_CANNOT_VOTE 120000
#endif

	MMatchObject* pObj = GetObject( uidPlayer );
	if( 0 == pObj )
		return;

	MCommand* pCmd = CreateCommand( MC_MATCH_VOTE_RESPONSE, MUID(0, 0) );
	if( 0 == pCmd )
		return;

	pCmd->AddParameter( new MCommandParameterInt(MERR_CANNOT_VOTE) );
	RouteToListener( pObj, pCmd );
}



void MMatchServer::OnEventChangeMaster(const MUID& uidAdmin)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if( 0 == pObj )
		return;

	if (!IsEnabledObject(pObj)) return;

	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	// ������ ������ ���� ����� �ƴϸ� ������ ���´�.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	if (pStage->GetMasterUID() == uidAdmin)
		return;

	pStage->SetMasterUID(uidAdmin);
	StageMaster(pStage->GetUID());
}

void MMatchServer::OnEventChangePassword(const MUID& uidAdmin, const char* pszPassword)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if( 0 == pObj ) 
		return;

	if (!IsEnabledObject(pObj)) return;

	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	// ������ ������ ���� ����� �ƴϸ� ������ ���´�.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	pStage->SetPassword(pszPassword);
	pStage->SetPrivate(true);
}

void MMatchServer::OnEventRequestJjang(const MUID& uidAdmin, const char* pszTargetName)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if( 0 == pObj )
		return;

	if (!IsEnabledObject(pObj)) return;

	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	// ������ ������ ���� ����� �ƴϸ� ����
	if (!IsAdminGrade(pObj))
	{
		return;
	}

	MMatchObject* pTargetObj = GetPlayerByName(pszTargetName);
	if (pTargetObj == NULL) return;
	if (IsAdminGrade(pTargetObj)) return;		// ���� ������� ¯�Ұ�
	if (MMUG_STAR == pTargetObj->GetAccountInfo()->m_nUGrade) return;	// �̹� ¯

	pTargetObj->GetAccountInfo()->m_nUGrade = MMUG_STAR;

	if (m_MatchDBMgr.EventJjangUpdate(pTargetObj->GetAccountInfo()->m_nAID, true)) {
		MMatchObjectCacheBuilder CacheBuilder;
		CacheBuilder.AddObject(pTargetObj);
		MCommand* pCmdCacheUpdate = CacheBuilder.GetResultCmd(MATCHCACHEMODE_REPLACE, this);
		RouteToStage(pStage->GetUID(), pCmdCacheUpdate);

		MCommand* pCmdUIUpdate = CreateCommand(MC_EVENT_UPDATE_JJANG, MUID(0,0));
		pCmdUIUpdate->AddParameter(new MCommandParameterUID(pTargetObj->GetUID()));
		pCmdUIUpdate->AddParameter(new MCommandParameterBool(true));
		RouteToStage(pStage->GetUID(), pCmdUIUpdate);
	}
}

void MMatchServer::OnEventRemoveJjang(const MUID& uidAdmin, const char* pszTargetName)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if( 0 == pObj )
		return;

	if (!IsEnabledObject(pObj)) return;

	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	// ������ ������ ���� ����� �ƴϸ� ������ ���´�.
	if (!IsAdminGrade(pObj))
	{
		return;
	}
	
	MMatchObject* pTargetObj = GetPlayerByName(pszTargetName);
	if (pTargetObj == NULL) return;			// ���� ������� ¯�Ұ�

	pTargetObj->GetAccountInfo()->m_nUGrade = MMUG_FREE;

	if (m_MatchDBMgr.EventJjangUpdate(pTargetObj->GetAccountInfo()->m_nAID, false)) {
		MMatchObjectCacheBuilder CacheBuilder;
		CacheBuilder.AddObject(pTargetObj);
		MCommand* pCmdCacheUpdate = CacheBuilder.GetResultCmd(MATCHCACHEMODE_REPLACE, this);
		RouteToStage(pStage->GetUID(), pCmdCacheUpdate);

		MCommand* pCmdUIUpdate = CreateCommand(MC_EVENT_UPDATE_JJANG, MUID(0,0));
		pCmdUIUpdate->AddParameter(new MCommandParameterUID(pTargetObj->GetUID()));
		pCmdUIUpdate->AddParameter(new MCommandParameterBool(false));
		RouteToStage(pStage->GetUID(), pCmdUIUpdate);
	}
}


void MMatchServer::OnStageGo(const MUID& uidPlayer, unsigned int nRoomNo)
{
	MMatchObject* pChar = GetObject(uidPlayer);
	if( 0 == pChar ) return;
	if (!IsEnabledObject(pChar)) return;
	if (pChar->GetPlace() != MMP_LOBBY) return;
	MMatchChannel* pChannel = FindChannel(pChar->GetChannelUID());
	if (pChannel == NULL) return;

	MMatchStage* pStage = pChannel->GetStage(nRoomNo-1);
	if (pStage) {
		MCommand* pNew = CreateCommand(MC_MATCH_REQUEST_STAGE_JOIN, GetUID());
		pNew->SetSenderUID(uidPlayer);	// �÷��̾ ���� �޽����� ��ó�� ����
		pNew->AddParameter(new MCommandParameterUID(uidPlayer));
		pNew->AddParameter(new MCommandParameterUID(pStage->GetUID()));
		Post(pNew);
	}
}



void MMatchServer::OnDuelQueueInfo(const MUID& uidStage, const MTD_DuelQueueInfo& QueueInfo)
{
	MCommand* pCmd = CreateCommand(MC_MATCH_DUEL_QUEUEINFO, MUID(0,0));
	pCmd->AddParameter(new MCmdParamBlob(&QueueInfo, sizeof(MTD_DuelQueueInfo)));
	RouteToBattle(uidStage, pCmd);
}


void MMatchServer::OnQuestSendPing(const MUID& uidStage, unsigned long int t)
{
	MCommand* pCmd = CreateCommand(MC_QUEST_PING, MUID(0,0));
	pCmd->AddParameter(new MCommandParameterUInt(t));
	RouteToBattle(uidStage, pCmd);
}

void MMatchServer::SaveGameLog(const MUID& uidStage)
{
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) return;

	int nMapID		= pStage->GetStageSetting()->GetMapIndex();
	int nGameType	= (int)pStage->GetStageSetting()->GetGameType();
	
	

	// test �ʵ��� �α� ������ �ʴ´�.
	if ( (MGetMapDescMgr()->MIsCorrectMap(nMapID)) && (MGetGameTypeMgr()->IsCorrectGameType(nGameType)) )
	{
		if (pStage->GetStageType() != MST_LADDER)
		{
			MMatchObject* pMaster = GetObject(pStage->GetMasterUID());

			MAsyncDBJob_InsertGameLog* pJob = new MAsyncDBJob_InsertGameLog(uidStage);
			pJob->Input(pMaster == NULL ? 0 : pMaster->GetCharInfo()->m_nCID,
				MGetMapDescMgr()->GetMapName(nMapID), 
				MGetGameTypeMgr()->GetInfo(MMATCH_GAMETYPE(nGameType))->szGameTypeStr);
			PostAsyncJob(pJob);
		}
	}

}

void MMatchServer::SaveGamePlayerLog(MMatchObject* pObj, unsigned int nStageID)
{	
	if( pObj == NULL ) return;
	if( nStageID == 0 ) return;
	if( pObj->GetCharInfo() == NULL ) return;

	MAsyncDBJob_InsertGamePlayerLog* pJob = new MAsyncDBJob_InsertGamePlayerLog;
	pJob->Input(nStageID, pObj->GetCharInfo()->m_nCID,
		(GetGlobalClockCount() - pObj->GetCharInfo()->m_nBattleStartTime) / 1000,
		pObj->GetCharInfo()->GetCharGamePlayInfo()->nKillCount,
		pObj->GetCharInfo()->GetCharGamePlayInfo()->nDeathCount,
		pObj->GetCharInfo()->GetCharGamePlayInfo()->nXP,
		pObj->GetCharInfo()->GetCharGamePlayInfo()->nBP);
	PostAsyncJob(pJob);
}


void MMatchServer::OnRequestFlagCap(const MUID& uidPlayer, const int nItemID)
{
	MMatchObject* pObj = GetObject(uidPlayer);
	if (pObj == NULL) return;
	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	if(pStage->GetStageSetting())
	{
		if(pStage->GetStageSetting()->GetGameType() == MMATCH_GAMETYPE_CTF)
		{
			MMatchRuleTeamCTF* pRule = (MMatchRuleTeamCTF*)pStage->GetRule();
			if(pRule)
			{
			pRule->OnObtainWorldItem( pObj, nItemID, NULL );
			}
		}
	}
}