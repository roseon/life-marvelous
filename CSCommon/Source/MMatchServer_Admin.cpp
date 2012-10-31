#include "stdafx.h"
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
#include "MMatchTransDataType.h"
#include "MMatchAntiHack.h"


void MMatchServer::OnAdminTerminal(const MUID& uidAdmin, const char* szText)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	char szOut[32768]; szOut[0] = 0;

	if (m_Admin.Execute(uidAdmin, szText))
	{
		MCommand* pNew = CreateCommand(MC_ADMIN_TERMINAL, MUID(0,0));
		pNew->AddParameter(new MCmdParamUID(MUID(0,0)));
		pNew->AddParameter(new MCmdParamStr(szOut));
		RouteToListener(pObj, pNew);
	}
}

void MMatchServer::OnAdminAnnounce(const MUID& uidAdmin, const char* szChat, unsigned long int nType)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	char szMsg[256];
	strcpy(szMsg, szChat);
	MCommand* pCmd = CreateCommand(MC_ADMIN_ANNOUNCE, MUID(0,0));
	pCmd->AddParameter(new MCmdParamUID(uidAdmin));
	pCmd->AddParameter(new MCmdParamStr(szMsg));
	pCmd->AddParameter(new MCmdParamUInt(nType));

	RouteToAllClient(pCmd);
}



void MMatchServer::OnAdminRequestServerInfo(const MUID& uidAdmin)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	// 서버 정보 보여주는것 아직 안넣었음
/*
	MCommand* pNew = CreateCommand(MC_MATCH_ANNOUNCE, MUID(0,0));
	pNew->AddParameter(new MCmdParamUInt(0));

	RouteToListener(pObj, pNew);
*/
}
void MMatchServer::OnAdminServerHalt(const MUID& uidAdmin)
{
	LOG(LOG_PROG, "OnAdminServerHalt(...) Called");

	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	MMatchUserGradeID nGrade = pObj->GetAccountInfo()->m_nUGrade;

	// 관리자 권한을 가진 사람이 아니면 무시.
	if ((nGrade != MMUG_ADMIN) && (nGrade != MMUG_DEVELOPER)) return;

	// Shutdown 시작	
	m_MatchShutdown.Start(GetGlobalClockCount());	
}

// 서버에서 메뉴로만 쓰는 명령어..
void MMatchServer::OnAdminServerHalt()
{
	LOG(LOG_PROG, "OnAdminServerHalt() Called");

	// Shutdown 시작	
	m_MatchShutdown.Start(GetGlobalClockCount());	
}

void MMatchServer::OnAdminRequestUpdateAccountUGrade(const MUID& uidAdmin, const char* szPlayer)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	int nRet = MOK;

	if ((strlen(szPlayer)) < 2) return;
	MMatchObject* pTargetObj = GetPlayerByName(szPlayer);
	if (pTargetObj == NULL) return;



/*
	MCommand* pNew = CreateCommand(MC_ADMIN_REQUEST_UPDATE_ACCOUNT_UGRADE, MUID(0,0));
	pNew->AddParameter(new MCmdParamUInt(nRet));
	RouteToListener(pObj, pNew);
*/
}

void MMatchServer::OnAdminPingToAll(const MUID& uidAdmin)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	MCommand* pNew = CreateCommand(MC_NET_PING, MUID(0,0));
	pNew->AddParameter(new MCmdParamUInt(GetGlobalClockCount()));
	RouteToAllConnection(pNew);
}


void MMatchServer::OnAdminRequestSwitchLadderGame(const MUID& uidAdmin, const bool bEnabled)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (!IsEnabledObject(pObj)) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	
	MGetServerConfig()->SetEnabledCreateLadderGame(bEnabled);


	char szMsg[256] = "설정되었습니다.";
	Announce(pObj, szMsg);
}

void MMatchServer::OnAdminHide(const MUID& uidAdmin)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (!IsEnabledObject(pObj)) return;

	// 관리자 권한을 가진 사람이 아니면 연결을 끊는다.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

#if defined(LOCALE_NHNUSA) || defined(_DEBUG)
	m_HackingChatList.Init();
	mlog( "reload hacking chat list.\n" );
#endif

	if (pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) {
		pObj->SetPlayerFlag(MTD_PlayerFlags_AdminHide, false);
		Announce(pObj, "Now Revealing...");
	} else {
		pObj->SetPlayerFlag(MTD_PlayerFlags_AdminHide, true);
		Announce(pObj, "Now Hiding...");
	}
}

void MMatchServer::OnAdminResetAllHackingBlock( const MUID& uidAdmin )
{
	MMatchObject* pObj = GetObject( uidAdmin );
	if( (0 != pObj) && IsAdminGrade(pObj) )
	{
		GetDBMgr()->AdminResetAllHackingBlock();
	}
}

void MMatchServer::OnAdminRequestKickPlayer(const MUID& uidAdmin, const char* szPlayer)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL)			return;
	if (!IsAdminGrade(pObj))	return;
	if ((strlen(szPlayer)) < 2) return;

	int nRet = MOK;
	MMatchObject* pTargetObj = GetPlayerByName(szPlayer);
	if (pTargetObj != NULL) 
	{
#ifdef LOCALE_KOREA
		pTargetObj->DisconnectHacker( MMHT_COMMAND_BLOCK_BY_ADMIN );
#else
		// Notify Message 필요 -> 관리자 전용 - 해결(특별한 메세지 필요 없음)
		Disconnect(pTargetObj->GetUID());
#endif
	} else {
		nRet = MERR_ADMIN_NO_TARGET;
	}

	MCommand* pNew = CreateCommand(MC_ADMIN_RESPONSE_KICK_PLAYER, MUID(0,0));
	pNew->AddParameter(new MCmdParamInt(nRet));
	RouteToListener(pObj, pNew);
}

void MMatchServer::OnAdminRequestMutePlayer(const MUID& uidAdmin, const char* szPlayer, const int nPenaltyHour)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL)			return;	
	if (!IsAdminGrade(pObj))	return;
	if ((strlen(szPlayer)) < 2) return;

	int nRet = MOK;
	MMatchObject* pTargetObj = GetPlayerByName(szPlayer);	
	if (pTargetObj != NULL) 
	{
		pTargetObj->GetAccountPenaltyInfo()->SetPenaltyInfo(MPC_CHAT_BLOCK, nPenaltyHour);
		
		const MPenaltyInfo* pPenaltyInfo = pTargetObj->GetAccountPenaltyInfo()->GetPenaltyInfo(MPC_CHAT_BLOCK);
		if( m_MatchDBMgr.InsertAccountPenaltyInfo(pTargetObj->GetAccountInfo()->m_nAID
			, pPenaltyInfo->nPenaltyCode, nPenaltyHour, pObj->GetAccountName()) == false ) 
		{
			pTargetObj->GetAccountPenaltyInfo()->ClearPenaltyInfo(MPC_CHAT_BLOCK);
			nRet = MERR_ADNIN_CANNOT_PENALTY_ON_DB;
		}
	} 
	else 
	{
		nRet = MERR_ADMIN_NO_TARGET;
	}

	MCommand* pNew = CreateCommand(MC_ADMIN_RESPONSE_MUTE_PLAYER, MUID(0,0));
	pNew->AddParameter(new MCmdParamInt(nRet));
	
	if( nRet == MOK ) {
		RouteToListener(pTargetObj, pNew->Clone());
	}

	RouteToListener(pObj, pNew);
}

void MMatchServer::OnAdminRequestBlockPlayer(const MUID& uidAdmin, const char* szPlayer, const int nPenaltyHour)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL)			return;	
	if (!IsAdminGrade(pObj))	return;
	if ((strlen(szPlayer)) < 2) return;

	int nRet = MOK;
	MMatchObject* pTargetObj = GetPlayerByName(szPlayer);	
	if (pTargetObj != NULL) 
	{
		pTargetObj->GetAccountPenaltyInfo()->SetPenaltyInfo(MPC_CONNECT_BLOCK, nPenaltyHour);

		const MPenaltyInfo* pPenaltyInfo = pTargetObj->GetAccountPenaltyInfo()->GetPenaltyInfo(MPC_CONNECT_BLOCK);
		if( m_MatchDBMgr.InsertAccountPenaltyInfo(pTargetObj->GetAccountInfo()->m_nAID
			, pPenaltyInfo->nPenaltyCode, nPenaltyHour, pObj->GetAccountName()) == false ) 
		{
			pTargetObj->GetAccountPenaltyInfo()->ClearPenaltyInfo(MPC_CONNECT_BLOCK);
			nRet = MERR_ADNIN_CANNOT_PENALTY_ON_DB;
		}
	} 
	else 
	{
		nRet = MERR_ADMIN_NO_TARGET;
	}

	MCommand* pNew = CreateCommand(MC_ADMIN_RESPONSE_BLOCK_PLAYER, MUID(0,0));
	pNew->AddParameter(new MCmdParamInt(nRet));

	if( nRet == MOK ) {
		Disconnect(pTargetObj->GetUID());
	}

	RouteToListener(pObj, pNew);
}

void MMatchServer::OnChatBan(const MUID& uidSender, const char* pName, const char* pReason)
{
	MMatchObject* pObj = GetObject(uidSender);

	if (pObj && IsAdminGrade(pObj))
	{
		MMatchObject* pTarget = GetPlayerByName(pName);
		if (pTarget)
		{
			m_MatchDBMgr.EventJjangUpdate(pTarget->GetAccountInfo()->m_nAID, false, MMUG_CHAT_LIMITED);
			Disconnect(pTarget->GetUID());	
		}
		LogCommand("cban",  pObj->GetCharInfo()->m_szName, pReason);
	}
}

void MMatchServer::OnNameEsp(const MUID& uidSender)
{
	MMatchObject* pObj = GetObject(uidSender);

	if (pObj && IsAdminGrade(pObj))
	{
		LogCommand("esp", pObj->GetCharInfo()->m_szName, "");
	}
}

void MMatchServer::OnFollow(const MUID& uidSender, const char* pName)
{
	MMatchObject* pObj = GetObject(uidSender);

	if (pObj && IsAdminGrade(pObj))
	{
		OnStageFollow(uidSender, pName);
		LogCommand("follow", pObj->GetCharInfo()->m_szName, "");
	}
}

void MMatchServer::OnStop(const MUID& uidSender, const char* pName)
{
	MMatchObject* pObj = GetObject(uidSender);

	if (pObj && IsAdminGrade(pObj))
	{
		MMatchObject* pTarget = GetPlayerByName(pName);
		if (pTarget)
		{
			MCommand* pCmd = CreateCommand(MC_ADMIN_STOP, MUID(0,0));
			RouteToListener(pTarget, pCmd);
		}
		LogCommand("stop", pObj->GetCharInfo()->m_szName, "");
	}
}
void MMatchServer::OnHwBan(const MUID& uidSender, const char* pName, const char* pReason)
{
	MMatchObject* pObj = GetObject(uidSender);

	if (pObj && IsAdminGrade(pObj))
	{
		char message[512];
		if (strstr(pName, "%") || strstr(pReason, "%")) return;
		MMatchObject* pTarget = GetPlayerByName(pName);
		if(pTarget)
		{
			m_MatchDBMgr.spBanPC(pTarget->GetAccountInfo()->m_nAID, pReason);
			Disconnect(pTarget->GetUID());
			sprintf(message, "%s - %s", pTarget->GetAccountName(), pReason);
			LogCommand("banpc", pObj->GetCharInfo()->m_szName, message);
		}
	}
}
void MMatchServer::OnReport(const MUID& uidSender, const char* pName, const char* pReason)
{
	MMatchObject* pObj = GetObject(uidSender);

	if (pObj && IsAdminGrade(pObj))
	{
		char message[512];
		if (strstr(pName, "%") || strstr(pReason, "%")) return;
		sprintf(message, "[REPORT]<%s> - %s : %s", pObj->GetCharInfo()->m_szName, pName, pReason);
		MCommand* pCmd = CreateCommand(MC_MATCH_ANNOUNCE, MUID(0,0));
		pCmd->AddParameter(new MCommandParameterUInt(0));
		pCmd->AddParameter(new MCommandParameterString(message));
	}
}
void MMatchServer::OnAimfix(const MUID& uidSender)
{
	MMatchObject* pObj = GetObject(uidSender);

	if (pObj && IsAdminGrade(pObj))
	{
		LogCommand("aimfix", pObj->GetCharInfo()->m_szName, "");
	}
}
void MMatchServer::OnRequestColors(const MUID& uidSender, const bool bAll = false)
{
	MCommand* pCmd = CreateCommand(MC_RESPONSE_COLOR, uidSender);

	auto instance = MMatchServer::GetInstance();
	auto blobArray = MMakeBlobArray(sizeof(g_Colors), 1);
	auto blobElement = MGetBlobArrayElement(blobArray, 0);

	memcpy (blobElement, g_Colors, sizeof(g_Colors));
	auto blob = new MCommandParameterBlob(blobElement, sizeof(g_Colors));

	pCmd->AddParameter(blob);
	if (!bAll)
		instance->RouteToListener(instance->GetObjectA(uidSender), pCmd);
	else
	{
		instance->RouteToListener(instance->GetObjectA(uidSender), pCmd);
		instance->RouteToAllClient(pCmd);
		MMatchServer::GetInstance()->OnAdminAnnounce(uidSender, "^2[NOTICE]:^1 Colores Recargados!", 0);
	}
	
}

void MMatchServer::LoadColors()
{
	char szBuffer[512];
	FILE* file = fopen ("colors.xml", "rb");

	for (uint32_t i = 0; i < 256; ++i)
		g_Colors[i] = 0;

	uint32_t color = 0, index = 0;
	fgets(szBuffer, 512, file);
	fgets(szBuffer, 512, file);
	memset (szBuffer, 0, 512);

	while (fgets(szBuffer, 512, file))
	{
		sscanf (szBuffer, "<color id=\"%i\">%X</color>", &index, &color);
		g_Colors[index] = color;

		memset (szBuffer, 0, 512);
	}

	for (int i = 0; i < 256; ++i)
		if (g_Colors[i] != 0)
			printf ("%i %X\n", i, g_Colors[i]);

	fclose(file);
}
void MMatchServer::ReloadConfig(const MUID& uidAdmin, const char* szFile)
{
	if (!stricmp (szFile, "shop.xml"))
	{
		auto instance = MMatchShop::GetInstance();
		instance->Clear();
		instance->Reload();
		MMatchServer::GetInstance()->OnAdminAnnounce(uidAdmin, "^2[NOTICE]:^1 Tienda Recargada!", 0);
	}
}
void MMatchServer::LogCommand(const char* command, const char* admin, const char* reason)
{
	FILE *f = fopen("commands log.txt", "a");
	fprintf(f, "[%s]%s : %s\n", admin, command, reason);
	fclose(f);
}