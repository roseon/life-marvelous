#ifndef _ZGLOBAL_H
#define _ZGLOBAL_H

//#include "RMesh.h"
#include "ZApplication.h"

// ���������� ����ϴ� ���� �̰����� �ֵ��� ����
#ifdef LOCALE_BRAZIL
#define APPLICATION_NAME		"GunZ Online"
#else
#define APPLICATION_NAME		"Gunz"
#endif

// ����Ǵ� ����
#ifdef LOCALE_BRAZIL
#define GUNZ_FOLDER			"/GunZ Online"
#elif LOCALE_JAPAN
#define GUNZ_FOLDER			"/GUNZWEI"
#else
#define GUNZ_FOLDER			"/Gunz"
#endif

#define SCREENSHOT_FOLDER	"/Screenshots"
#define REPLAY_FOLDER		"/Replay"
#define EMBLEM_FOLDER		"/Emblem"


class MZFileSystem;
class MMessenger;

class ZApplication;
class ZGameClient;
class ZSoundEngine;
class ZGameInterface;
class ZEffectManager;
class ZScreenEffectManager;
class ZDirectInput;
class ZCombatInterface;
class ZCamera;
class ZGame;
class ZBaseQuest;
class ZQuest;
class ZSurvival;
class ZGameTypeManager;
class ZWorldManager;
class ZMessengerManager;
class ZEmblemInterface;
class ZInput;

extern ZDirectInput	g_DInput;
extern ZInput* g_pInput;

bool					ZIsLaunchDevelop(void);

RMeshMgr*				ZGetNpcMeshMgr(void);
RMeshMgr*				ZGetMeshMgr(void);
RMeshMgr*				ZGetWeaponMeshMgr(void);
RAniEventMgr*			ZGetAniEventMgr(void);

ZSoundEngine*			ZGetSoundEngine(void);

ZEffectManager*			ZGetEffectManager(void);
ZScreenEffectManager*	ZGetScreenEffectManager(void);

int						ZGetSVNRevision(void);


// dll-injection���� ȣ���ϴ� �� ������ ��ũ�� �ζ��̴�
#define ZGetApplication()		ZApplication::GetInstance()
#define ZGetGameClient()		(ZApplication::GetGameInterface() ? ZApplication::GetGameInterface()->GetGameClient() : NULL)
#define ZGetGame()				(ZApplication::GetGameInterface() ? ZApplication::GetGameInterface()->GetGame() : NULL)

#define ZGetGameInterface()		ZApplication::GetGameInterface()
#define ZGetCombatInterface()	(ZApplication::GetGameInterface() ? ZApplication::GetGameInterface()->GetCombatInterface() : NULL)

#define ZGetFileSystem()		ZApplication::GetFileSystem()
#define ZGetDirectInput()		(&g_DInput)

#define ZGetQuest()				((ZBaseQuest*)((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetQuest() : NULL))
#define ZGetQuestExactly()		((ZQuest*)((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetQuestExactly() : NULL))
#define ZGetSurvivalExactly()	((ZSurvival*)((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetSurvivalExactly() : NULL))

#define ZGetGameTypeManager()	((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetGameTypeManager() : NULL)

#define ZGetInput()				(g_pInput)
#define ZGetCamera()			(ZApplication::GetGameInterface() ? ZApplication::GetGameInterface()->GetCamera() : NULL)

#define ZGetWorldManager()		ZApplication::GetInstance()->GetWorldManager()
#define ZGetWorld()				(ZGetWorldManager()->GetCurrent())

inline ZEmblemInterface*	ZGetEmblemInterface() { return ZApplication::GetInstance()->GetEmblemInterface(); }
inline ZOptionInterface*	ZGetOptionInterface(void) { return ZApplication::GetInstance()->GetOptionInterface(); }


#define ZIsActionKeyPressed(_ActionID)	(ZGetInput()->IsActionKeyPressed(_ActionID))

//jintriple3 ��ũ��..
#define PROTECT_DEBUG_REGISTER(b) if(GetTickCount() >0)if(GetTickCount() >0)if(GetTickCount() >0)if(b)
//jintriple3 ����� �������� ��ŷ ��� ���� �� ����.
#define FOR_DEBUG_REGISTER 1000


#endif