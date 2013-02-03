#include "stdafx.h"
#include "ZCommandUDPHackShield.h"


void ZCommandUDPHackShield::Init()
{
	// peer2peer를 제외한 거의 모든 커맨드들

	AddDeniedCommand(MC_HELP);
	AddDeniedCommand(MC_VERSION);
	AddDeniedCommand(MC_DEBUG_TEST);
	AddDeniedCommand(MC_LOCAL_INFO);
	AddDeniedCommand(MC_LOCAL_ECHO);
	AddDeniedCommand(MC_LOCAL_LOGIN);

	AddDeniedCommand(MC_NET_ENUM);
	AddDeniedCommand(MC_NET_CONNECT);
	AddDeniedCommand(MC_NET_DISCONNECT);
	AddDeniedCommand(MC_NET_ONCONNECT);
	AddDeniedCommand(MC_NET_ONDISCONNECT);

	AddDeniedCommand(MC_NET_CONNECTTOZONESERVER);

	AddDeniedCommand(MC_MATCH_NOTIFY);
	AddDeniedCommand(MC_MATCH_ANNOUNCE);
	AddDeniedCommand(MC_MATCH_RESPONSE_RESULT);

	AddDeniedCommand(MC_ADMIN_ANNOUNCE);
	AddDeniedCommand(MC_ADMIN_FORUM_ANNOUNCE);
	AddDeniedCommand(MC_ADMIN_REQUEST_SERVER_INFO);
	AddDeniedCommand(MC_ADMIN_RESPONSE_SERVER_INFO);
	AddDeniedCommand(MC_ADMIN_SERVER_HALT);
	AddDeniedCommand(MC_ADMIN_TERMINAL);
	AddDeniedCommand(MC_ADMIN_REQUEST_UPDATE_ACCOUNT_UGRADE);
	AddDeniedCommand(MC_ADMIN_RESPONSE_UPDATE_ACCOUNT_UGRADE);
	AddDeniedCommand(MC_ADMIN_REQUEST_KICK_PLAYER);
	AddDeniedCommand(MC_ADMIN_RESPONSE_KICK_PLAYER);
	AddDeniedCommand(MC_ADMIN_REQUEST_MUTE_PLAYER);
	AddDeniedCommand(MC_ADMIN_RESPONSE_MUTE_PLAYER);
	AddDeniedCommand(MC_ADMIN_REQUEST_BLOCK_PLAYER);
	AddDeniedCommand(MC_ADMIN_RESPONSE_BLOCK_PLAYER);
	AddDeniedCommand(MC_ADMIN_PING_TO_ALL);
	AddDeniedCommand(MC_ADMIN_REQUEST_SWITCH_LADDER_GAME);
	AddDeniedCommand(MC_ADMIN_HIDE);
	AddDeniedCommand(MC_ADMIN_RELOAD_CLIENT_HASH);
	AddDeniedCommand(MC_ADMIN_RESET_ALL_HACKING_BLOCK);
	AddDeniedCommand(MC_ADMIN_RELOAD_GAMBLEITEM);
	AddDeniedCommand(MC_ADMIN_DUMP_GAMBLEITEM_LOG);
	AddDeniedCommand(MC_ADMIN_ASSASIN);

	AddDeniedCommand(MC_EVENT_CHANGE_MASTER);
	AddDeniedCommand(MC_EVENT_CHANGE_PASSWORD);
	AddDeniedCommand(MC_EVENT_REQUEST_JJANG);
	AddDeniedCommand(MC_EVENT_REMOVE_JJANG);
	AddDeniedCommand(MC_EVENT_UPDATE_JJANG);

	AddDeniedCommand(MC_MATCH_LOGIN);
	AddDeniedCommand(MC_MATCH_RESPONSE_LOGIN);
	AddDeniedCommand(MC_MATCH_LOGIN_NETMARBLE);
	AddDeniedCommand(MC_MATCH_LOGIN_NETMARBLE_JP);
	AddDeniedCommand(MC_MATCH_LOGIN_FROM_DBAGENT);
	AddDeniedCommand(MC_MATCH_LOGIN_FROM_DBAGENT_FAILED);
	AddDeniedCommand(MC_MATCH_FIND_HACKING);
	AddDeniedCommand(MC_MATCH_DISCONNMSG);
	AddDeniedCommand(MC_MATCH_LOGIN_NHNUSA);
	AddDeniedCommand(MC_MATCH_LOGIN_GAMEON_JP);

	AddDeniedCommand(MC_MATCH_BRIDGEPEER);
	AddDeniedCommand(MC_MATCH_BRIDGEPEER_ACK);

	AddDeniedCommand(MC_MATCH_OBJECT_CACHE);

	AddDeniedCommand(MC_MATCH_REQUEST_RECOMMANDED_CHANNEL);
	AddDeniedCommand(MC_MATCH_RESPONSE_RECOMMANDED_CHANNEL);
	AddDeniedCommand(MC_MATCH_CHANNEL_REQUEST_JOIN);
	AddDeniedCommand(MC_MATCH_CHANNEL_REQUEST_JOIN_FROM_NAME);
	AddDeniedCommand(MC_MATCH_CHANNEL_RESPONSE_JOIN);
	AddDeniedCommand(MC_MATCH_CHANNEL_LEAVE);
	AddDeniedCommand(MC_MATCH_CHANNEL_LIST_START);
	AddDeniedCommand(MC_MATCH_CHANNEL_LIST_STOP);
	AddDeniedCommand(MC_MATCH_CHANNEL_LIST);
	AddDeniedCommand(MC_MATCH_CHANNEL_REQUEST_PLAYER_LIST);
	AddDeniedCommand(MC_MATCH_CHANNEL_RESPONSE_PLAYER_LIST);
	AddDeniedCommand(MC_MATCH_CHANNEL_REQUEST_CHAT);
	AddDeniedCommand(MC_MATCH_CHANNEL_CHAT);
	AddDeniedCommand(MC_MATCH_CHANNEL_DUMB_CHAT);

	AddDeniedCommand(MC_MATCH_CHANNEL_REQUEST_RULE);
	AddDeniedCommand(MC_MATCH_CHANNEL_RESPONSE_RULE);
	AddDeniedCommand(MC_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST);
	AddDeniedCommand(MC_MATCH_CHANNEL_RESPONSE_ALL_PLAYER_LIST);

	AddDeniedCommand(MC_MATCH_STAGE_CREATE);
	AddDeniedCommand(MC_MATCH_RESPONSE_STAGE_CREATE);
	AddDeniedCommand(MC_MATCH_STAGE_JOIN);
	AddDeniedCommand(MC_MATCH_REQUEST_STAGE_JOIN);
	AddDeniedCommand(MC_MATCH_REQUEST_PRIVATE_STAGE_JOIN);
	AddDeniedCommand(MC_MATCH_RESPONSE_STAGE_JOIN);
	AddDeniedCommand(MC_MATCH_STAGE_LEAVE);
	AddDeniedCommand(MC_MATCH_STAGE_REQUEST_PLAYERLIST);
	AddDeniedCommand(MC_MATCH_STAGE_FOLLOW);
	AddDeniedCommand(MC_MATCH_RESPONSE_STAGE_FOLLOW);
	AddDeniedCommand(MC_MATCH_REQUEST_STAGE_LIST);
	AddDeniedCommand(MC_MATCH_STAGE_LIST_START);
	AddDeniedCommand(MC_MATCH_STAGE_LIST_STOP);
	AddDeniedCommand(MC_MATCH_STAGE_LIST);
	AddDeniedCommand(MC_MATCH_STAGE_CHAT);
	AddDeniedCommand(MC_MATCH_STAGE_REQUEST_QUICKJOIN);
	AddDeniedCommand(MC_MATCH_STAGE_RESPONSE_QUICKJOIN);
	AddDeniedCommand(MC_MATCH_STAGE_GO);
	AddDeniedCommand(MC_MATCH_STAGE_REQUIRE_PASSWORD);

	AddDeniedCommand(MC_MATCH_STAGE_REQUEST_ENTERBATTLE);
	AddDeniedCommand(MC_MATCH_STAGE_ENTERBATTLE);
	AddDeniedCommand(MC_MATCH_STAGE_LEAVEBATTLE_TO_SERVER);
	AddDeniedCommand(MC_MATCH_STAGE_LEAVEBATTLE_TO_CLIENT);

	AddDeniedCommand(MC_MATCH_REQUEST_STAGESETTING);
	AddDeniedCommand(MC_MATCH_RESPONSE_STAGESETTING);
	AddDeniedCommand(MC_MATCH_STAGESETTING);
	AddDeniedCommand(MC_MATCH_STAGE_MAP);
	AddDeniedCommand(MC_MATCH_STAGE_REQUEST_FORCED_ENTRY);
	AddDeniedCommand(MC_MATCH_STAGE_RESPONSE_FORCED_ENTRY);
	AddDeniedCommand(MC_MATCH_STAGE_RELAY_MAP_ELEMENT_UPDATE);
	AddDeniedCommand(MC_MATCH_STAGE_RELAY_MAP_INFO_UPDATE);

	AddDeniedCommand(MC_MATCH_STAGE_MASTER);
	AddDeniedCommand(MC_MATCH_STAGE_PLAYER_STATE);
	AddDeniedCommand(MC_MATCH_STAGE_TEAM);
	AddDeniedCommand(MC_MATCH_STAGE_START);
	AddDeniedCommand(MC_MATCH_STAGE_LAUNCH);
	AddDeniedCommand(MC_MATCH_STAGE_RELAY_LAUNCH);

	AddDeniedCommand(MC_MATCH_LOADING_COMPLETE);
	AddDeniedCommand(MC_MATCH_STAGE_FINISH_GAME);

	AddDeniedCommand(MC_MATCH_REQUEST_GAME_INFO);
	AddDeniedCommand(MC_MATCH_RESPONSE_GAME_INFO);

	AddDeniedCommand(MC_MATCH_REQUEST_PEERLIST);
	AddDeniedCommand(MC_MATCH_RESPONSE_PEERLIST);

	AddDeniedCommand(MC_MATCH_REQUEST_PEER_RELAY);
	AddDeniedCommand(MC_MATCH_RESPONSE_PEER_RELAY);

	AddDeniedCommand(MC_MATCH_GAME_ROUNDSTATE);
	AddDeniedCommand(MC_MATCH_ROUND_FINISHINFO);
	AddDeniedCommand(MC_MATCH_GAME_KILL);
	AddDeniedCommand(MC_MATCH_GAME_DEAD);
	AddDeniedCommand(MC_MATCH_GAME_LEVEL_UP);
	AddDeniedCommand(MC_MATCH_GAME_LEVEL_DOWN);
	AddDeniedCommand(MC_MATCH_GAME_REQUEST_SPAWN);
	AddDeniedCommand(MC_MATCH_GAME_RESPONSE_SPAWN);
	AddDeniedCommand(MC_MATCH_GAME_TEAMBONUS);

	AddDeniedCommand(MC_MATCH_GAME_REQUEST_TIMESYNC);
	AddDeniedCommand(MC_MATCH_GAME_RESPONSE_TIMESYNC);
	AddDeniedCommand(MC_MATCH_GAME_REPORT_TIMESYNC);

	AddDeniedCommand(MC_MATCH_REQUEST_SUICIDE);
	AddDeniedCommand(MC_MATCH_RESPONSE_SUICIDE);
	AddDeniedCommand(MC_MATCH_RESPONSE_SUICIDE_RESERVE);
	AddDeniedCommand(MC_MATCH_REQUEST_OBTAIN_WORLDITEM);
	AddDeniedCommand(MC_MATCH_OBTAIN_WORLDITEM);
	AddDeniedCommand(MC_MATCH_SPAWN_WORLDITEM);
	AddDeniedCommand(MC_MATCH_REQUEST_SPAWN_WORLDITEM);
	AddDeniedCommand(MC_MATCH_REMOVE_WORLDITEM);
	AddDeniedCommand(MC_MATCH_ASSIGN_COMMANDER);
	AddDeniedCommand(MC_MATCH_RESET_TEAM_MEMBERS);
	AddDeniedCommand(MC_MATCH_SET_OBSERVER);

	AddDeniedCommand(MC_MATCH_REQUEST_PROPOSAL);
	AddDeniedCommand(MC_MATCH_RESPONSE_PROPOSAL);
	AddDeniedCommand(MC_MATCH_ASK_AGREEMENT);
	AddDeniedCommand(MC_MATCH_REPLY_AGREEMENT);

	AddDeniedCommand(MC_MATCH_LADDER_REQUEST_CHALLENGE);
	AddDeniedCommand(MC_MATCH_LADDER_RESPONSE_CHALLENGE);
	AddDeniedCommand(MC_MATCH_LADDER_SEARCH_RIVAL);
	AddDeniedCommand(MC_MATCH_LADDER_REQUEST_CANCEL_CHALLENGE);
	AddDeniedCommand(MC_MATCH_LADDER_CANCEL_CHALLENGE);
	AddDeniedCommand(MC_MATCH_LADDER_PREPARE);
	AddDeniedCommand(MC_MATCH_LADDER_LAUNCH);

	AddDeniedCommand(MC_MATCH_USER_WHISPER);
	AddDeniedCommand(MC_MATCH_USER_WHERE);
	AddDeniedCommand(MC_MATCH_USER_OPTION);
	AddDeniedCommand(MC_MATCH_CHATROOM_CREATE);
	AddDeniedCommand(MC_MATCH_CHATROOM_JOIN);
	AddDeniedCommand(MC_MATCH_CHATROOM_LEAVE);
	AddDeniedCommand(MC_MATCH_CHATROOM_INVITE);
	AddDeniedCommand(MC_MATCH_CHATROOM_CHAT);
	AddDeniedCommand(MC_MATCH_CHATROOM_SELECT_WRITE);

	AddDeniedCommand(MC_MATCH_REQUEST_ACCOUNT_CHARLIST);
	AddDeniedCommand(MC_MATCH_RESPONSE_ACCOUNT_CHARLIST);
	AddDeniedCommand(MC_MATCH_REQUEST_SELECT_CHAR);
	AddDeniedCommand(MC_MATCH_RESPONSE_SELECT_CHAR);
	AddDeniedCommand(MC_MATCH_REQUEST_MYCHARINFO);
	AddDeniedCommand(MC_MATCH_RESPONSE_MYCHARINFO);
	AddDeniedCommand(MC_MATCH_REQUEST_CREATE_CHAR);
	AddDeniedCommand(MC_MATCH_RESPONSE_CREATE_CHAR);
	AddDeniedCommand(MC_MATCH_REQUEST_DELETE_CHAR);
	AddDeniedCommand(MC_MATCH_RESPONSE_DELETE_CHAR);
	AddDeniedCommand(MC_MATCH_REQUEST_CHARINFO_DETAIL);
	AddDeniedCommand(MC_MATCH_RESPONSE_CHARINFO_DETAIL);
	AddDeniedCommand(MC_MATCH_REQUEST_ACCOUNT_CHARINFO);
	AddDeniedCommand(MC_MATCH_RESPONSE_ACCOUNT_CHARINFO);

	AddDeniedCommand(MC_MATCH_REQUEST_SIMPLE_CHARINFO);
	AddDeniedCommand(MC_MATCH_RESPONSE_SIMPLE_CHARINFO);
	AddDeniedCommand(MC_MATCH_REQUEST_MY_SIMPLE_CHARINFO);
	AddDeniedCommand(MC_MATCH_RESPONSE_MY_SIMPLE_CHARINFO);

	AddDeniedCommand(MC_MATCH_REQUEST_BUY_ITEM);
	AddDeniedCommand(MC_MATCH_RESPONSE_BUY_ITEM);
	AddDeniedCommand(MC_MATCH_REQUEST_SELL_ITEM);
	AddDeniedCommand(MC_MATCH_RESPONSE_SELL_ITEM);
	AddDeniedCommand(MC_MATCH_REQUEST_SHOP_ITEMLIST);
	AddDeniedCommand(MC_MATCH_RESPONSE_SHOP_ITEMLIST);

	AddDeniedCommand(MC_MATCH_REQUEST_CHARACTER_ITEMLIST);
	AddDeniedCommand(MC_MATCH_RESPONSE_CHARACTER_ITEMLIST);
	AddDeniedCommand(MC_MATCH_REQUEST_EQUIP_ITEM);
	AddDeniedCommand(MC_MATCH_RESPONSE_EQUIP_ITEM);
	AddDeniedCommand(MC_MATCH_REQUEST_TAKEOFF_ITEM);
	AddDeniedCommand(MC_MATCH_RESPONSE_TAKEOFF_ITEM);
	AddDeniedCommand(MC_RESPONSE_GAMBLEITEMLIST);
	AddDeniedCommand(MC_MATCH_ROUTE_UPDATE_STAGE_EQUIP_LOOK);
	AddDeniedCommand(MC_MATCH_REQUEST_CHARACTER_ITEMLIST_FORCE);

	AddDeniedCommand(MC_MATCH_REQUEST_ACCOUNT_ITEMLIST);
	AddDeniedCommand(MC_MATCH_RESPONSE_ACCOUNT_ITEMLIST);
	AddDeniedCommand(MC_MATCH_REQUEST_BRING_ACCOUNTITEM);
	AddDeniedCommand(MC_MATCH_RESPONSE_BRING_ACCOUNTITEM);
	AddDeniedCommand(MC_MATCH_REQUEST_BRING_BACK_ACCOUNTITEM);
	AddDeniedCommand(MC_MATCH_RESPONSE_BRING_BACK_ACCOUNTITEM);
	AddDeniedCommand(MC_MATCH_EXPIRED_RENT_ITEM);

	AddDeniedCommand(MC_MATCH_REQUEST_GAMBLE);
	AddDeniedCommand(MC_MATCH_RESPONSE_GAMBLE);

	AddDeniedCommand(MC_MATCH_FRIEND_ADD);
	AddDeniedCommand(MC_MATCH_FRIEND_REMOVE);
	AddDeniedCommand(MC_MATCH_FRIEND_LIST);
	AddDeniedCommand(MC_MATCH_RESPONSE_FRIENDLIST);
	AddDeniedCommand(MC_MATCH_FRIEND_MSG);

	AddDeniedCommand(MC_MATCH_CLAN_REQUEST_CREATE_CLAN);
	AddDeniedCommand(MC_MATCH_CLAN_RESPONSE_CREATE_CLAN);
	AddDeniedCommand(MC_MATCH_CLAN_ASK_SPONSOR_AGREEMENT);
	AddDeniedCommand(MC_MATCH_CLAN_ANSWER_SPONSOR_AGREEMENT);
	AddDeniedCommand(MC_MATCH_CLAN_REQUEST_AGREED_CREATE_CLAN);
	AddDeniedCommand(MC_MATCH_CLAN_RESPONSE_AGREED_CREATE_CLAN);
	AddDeniedCommand(MC_MATCH_CLAN_REQUEST_CLOSE_CLAN);
	AddDeniedCommand(MC_MATCH_CLAN_RESPONSE_CLOSE_CLAN);
	AddDeniedCommand(MC_MATCH_CLAN_REQUEST_JOIN_CLAN);
	AddDeniedCommand(MC_MATCH_CLAN_RESPONSE_JOIN_CLAN);
	AddDeniedCommand(MC_MATCH_CLAN_ASK_JOIN_AGREEMENT);
	AddDeniedCommand(MC_MATCH_CLAN_ANSWER_JOIN_AGREEMENT);
	AddDeniedCommand(MC_MATCH_CLAN_REQUEST_AGREED_JOIN_CLAN);
	AddDeniedCommand(MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN);
	AddDeniedCommand(MC_MATCH_CLAN_REQUEST_LEAVE_CLAN);
	AddDeniedCommand(MC_MATCH_CLAN_RESPONSE_LEAVE_CLAN);
	AddDeniedCommand(MC_MATCH_CLAN_UPDATE_CHAR_CLANINFO);
	AddDeniedCommand(MC_MATCH_CLAN_MASTER_REQUEST_CHANGE_GRADE);
	AddDeniedCommand(MC_MATCH_CLAN_MASTER_RESPONSE_CHANGE_GRADE);
	AddDeniedCommand(MC_MATCH_CLAN_ADMIN_REQUEST_EXPEL_MEMBER);
	AddDeniedCommand(MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER);
	AddDeniedCommand(MC_MATCH_CLAN_REQUEST_MSG);
	AddDeniedCommand(MC_MATCH_CLAN_MSG);
	AddDeniedCommand(MC_MATCH_CLAN_REQUEST_MEMBER_LIST);
	AddDeniedCommand(MC_MATCH_CLAN_RESPONSE_MEMBER_LIST);
	AddDeniedCommand(MC_MATCH_CLAN_REQUEST_CLAN_INFO);
	AddDeniedCommand(MC_MATCH_CLAN_RESPONSE_CLAN_INFO);
	AddDeniedCommand(MC_MATCH_CLAN_STANDBY_CLAN_LIST);
	AddDeniedCommand(MC_MATCH_CLAN_MEMBER_CONNECTED);
	AddDeniedCommand(MC_MATCH_CLAN_REQUEST_EMBLEMURL);
	AddDeniedCommand(MC_MATCH_CLAN_RESPONSE_EMBLEMURL);
	AddDeniedCommand(MC_MATCH_CLAN_LOCAL_EMBLEMREADY);
	AddDeniedCommand(MC_MATCH_CLAN_ACCOUNCE_DELETE);

	AddDeniedCommand(MC_MATCH_CALLVOTE);
	AddDeniedCommand(MC_MATCH_NOTIFY_CALLVOTE);
	AddDeniedCommand(MC_MATCH_NOTIFY_VOTERESULT);
	AddDeniedCommand(MC_MATCH_VOTE_YES);
	AddDeniedCommand(MC_MATCH_VOTE_NO);
	AddDeniedCommand(MC_MATCH_VOTE_RESPONSE);
	AddDeniedCommand(MC_MATCH_VOTE_STOP);

	AddDeniedCommand(MC_MATCH_BROADCAST_CLAN_RENEW_VICTORIES);
	AddDeniedCommand(MC_MATCH_BROADCAST_CLAN_INTERRUPT_VICTORIES);
	AddDeniedCommand(MC_MATCH_BROADCAST_DUEL_RENEW_VICTORIES);
	AddDeniedCommand(MC_MATCH_BROADCAST_DUEL_INTERRUPT_VICTORIES);

	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_REQUEST_JOINGAME);
	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_RESPONSE_JOINGAME);
	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_REQUEST_CANCELGAME);
	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_CHAR_INFO);
	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_PREPARE_MATCH);
	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_LAUNCH_MATCH);
	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_CANCEL_MATCH);
	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_CHAR_INFO_PREVIOUS);
	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_NOT_SERVICE_TIME);

	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_REQUEST_SIDERANKING_INFO);
	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_RESPONSE_SIDERANKING_INFO);
	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_GROUPRANKING_INFO);

	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_GAME_INFO);
	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_GAME_ROUND_RESULT_INFO);
	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_GAME_MATCH_RESULT_INFO);
	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_GAME_PLAYER_STATUS);
	AddDeniedCommand(MC_MATCH_DUELTOURNAMENT_GAME_NEXT_MATCH_PLYAERINFO);

	AddDeniedCommand(MC_MATCH_ASSIGN_BERSERKER);

	AddDeniedCommand(MC_MATCH_DUEL_QUEUEINFO);

	AddDeniedCommand(MC_MATCH_REGISTERAGENT);
	AddDeniedCommand(MC_MATCH_UNREGISTERAGENT);
	AddDeniedCommand(MC_MATCH_AGENT_REQUEST_LIVECHECK);
	AddDeniedCommand(MC_MATCH_AGENT_RESPONSE_LIVECHECK);
	AddDeniedCommand(MC_AGENT_ERROR);

	AddDeniedCommand(MC_QUEST_NPC_SPAWN);
	AddDeniedCommand(MC_QUEST_ENTRUST_NPC_CONTROL);
	AddDeniedCommand(MC_QUEST_CHECKSUM_NPCINFO);
	AddDeniedCommand(MC_QUEST_REQUEST_NPC_DEAD);
	AddDeniedCommand(MC_QUEST_NPC_DEAD);
	AddDeniedCommand(MC_QUEST_REFRESH_PLAYER_STATUS);
	AddDeniedCommand(MC_QUEST_NPC_ALL_CLEAR);
	AddDeniedCommand(MC_MATCH_QUEST_REQUEST_DEAD);
	AddDeniedCommand(MC_MATCH_QUEST_PLAYER_DEAD);
	AddDeniedCommand(MC_QUEST_OBTAIN_QUESTITEM);
	AddDeniedCommand(MC_QUEST_STAGE_MAPSET);
	AddDeniedCommand(MC_QUEST_OBTAIN_ZITEM);
	AddDeniedCommand(MC_QUEST_PING);
	AddDeniedCommand(MC_QUEST_PONG);
	AddDeniedCommand(MC_QUEST_NPCLIST);

	AddDeniedCommand(MC_QUEST_GAME_INFO);
	AddDeniedCommand(MC_QUEST_COMBAT_STATE);
	AddDeniedCommand(MC_QUEST_SECTOR_START);
	AddDeniedCommand(MC_QUEST_COMPLETED);
	AddDeniedCommand(MC_QUEST_FAILED);

	AddDeniedCommand(MC_QUEST_REQUEST_MOVETO_PORTAL);
	AddDeniedCommand(MC_QUEST_MOVETO_PORTAL);
	AddDeniedCommand(MC_QUEST_READYTO_NEWSECTOR);

	AddDeniedCommand(MC_GAME_START_FAIL);
	AddDeniedCommand(MC_QUEST_STAGE_GAME_INFO);
	AddDeniedCommand(MC_QUEST_SECTOR_BONUS);

	AddDeniedCommand(MC_QUEST_ROUND_START);
	AddDeniedCommand(MC_QUEST_REQUEST_QL);
	AddDeniedCommand(MC_QUEST_RESPONSE_QL);
	AddDeniedCommand(MC_QUEST_SURVIVAL_RESULT);
	AddDeniedCommand(MC_SURVIVAL_RANKINGLIST);
	AddDeniedCommand(MC_SURVIVAL_PRIVATERANKING);

	AddDeniedCommand(MC_QUEST_TEST_REQUEST_NPC_SPAWN);
	AddDeniedCommand(MC_QUEST_TEST_REQUEST_CLEAR_NPC);
	AddDeniedCommand(MC_QUEST_TEST_REQUEST_SECTOR_CLEAR);
	AddDeniedCommand(MC_QUEST_TEST_REQUEST_FINISH);

	AddDeniedCommand(MC_MATCH_SCHEDULE_ANNOUNCE_MAKE);
	AddDeniedCommand(MC_MATCH_SCHEDULE_ANNOUNCE_SEND);
	AddDeniedCommand(MC_MATCH_SCHEDULE_CLAN_SERVER_SWITCH_DOWN);
	AddDeniedCommand(MC_MATCH_SCHEDULE_CLAN_SERVER_SWITCH_ON);
	AddDeniedCommand(MC_MATCH_SCHEDULE_STOP_SERVER);

	AddDeniedCommand(MC_REQUEST_GAMEGUARD_AUTH);
	AddDeniedCommand(MC_RESPONSE_GAMEGUARD_AUTH);
	AddDeniedCommand(MC_REQUEST_FIRST_GAMEGUARD_AUTH);
	AddDeniedCommand(MC_RESPONSE_FIRST_GAMEGUARD_AUTH);

	AddDeniedCommand(MC_REQUEST_GIVE_ONESELF_UP);

	AddDeniedCommand(MC_REQUEST_RESOURCE_CRC32);
	AddDeniedCommand(MC_RESPONSE_RESOURCE_CRC32);

	AddDeniedCommand(MC_MATCH_REQUEST_CHAR_QUEST_ITEM_LIST);
	AddDeniedCommand(MC_MATCH_RESPONSE_CHAR_QUEST_ITEM_LIST);
	AddDeniedCommand(MC_MATCH_REQUEST_BUY_QUEST_ITEM);
	AddDeniedCommand(MC_MATCH_RESPONSE_BUY_QUEST_ITEM);
	AddDeniedCommand(MC_MATCH_REQUEST_SELL_QUEST_ITEM);
	AddDeniedCommand(MC_MATCH_RESPONSE_SELL_QUEST_ITEM);
	AddDeniedCommand(MC_MATCH_USER_REWARD_QUEST);
	AddDeniedCommand(MC_MATCH_REQUEST_DROP_SACRIFICE_ITEM);
	AddDeniedCommand(MC_MATCH_RESPONSE_DROP_SACRIFICE_ITEM);
	AddDeniedCommand(MC_MATCH_REQUEST_CALLBACK_SACRIFICE_ITEM);
	AddDeniedCommand(MC_MATCH_RESPONSE_CALLBACK_SACRIFICE_ITEM);
	AddDeniedCommand(MC_MATCH_REQUEST_SLOT_INFO);
	AddDeniedCommand(MC_MATCH_RESPONSE_SLOT_INFO);
	AddDeniedCommand(MC_MATCH_NEW_MONSTER_INFO);
	AddDeniedCommand(MC_MATCH_REQUEST_MONSTER_BIBLE_INFO);
	AddDeniedCommand(MC_MATCH_RESPONSE_MONSTER_BIBLE_INFO);
	AddDeniedCommand(MC_MATCH_FLAG_EFFECT);
    AddDeniedCommand(MC_MATCH_FLAG_CAP);
	AddDeniedCommand(MC_MATCH_SKILFLAG_CAP);
    AddDeniedCommand(MC_MATCH_FLAG_STATE);
}

void ZCommandUDPHackShield::AddDeniedCommand(int nCommandID)
{
	if( IsDeniedCommand(nCommandID) ) {
		_ASSERT(0);
		return;
	}

	m_UDPDeniedCommand.insert(nCommandID);
}

bool ZCommandUDPHackShield::IsDeniedCommand(int nCommandID)
{
	set<int>::iterator it = m_UDPDeniedCommand.find(nCommandID);
	if (it != m_UDPDeniedCommand.end())
		return true;
	return false;
}