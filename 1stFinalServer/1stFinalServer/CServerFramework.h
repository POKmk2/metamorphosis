#pragma once
#include "CRoomManager.h"
#include "CIOCP.h"
class CServerFramework
{
public:
	CRoomManager m_roomManager;
	CIOCP m_IOCP;
	std::chrono::system_clock::time_point m_start;
	WSADATA	wsadata;

	CServerFramework();
	~CServerFramework();
	void update();
	void initiate();

};


void accept_thread(CServerFramework* server);
void worker_thread(CServerFramework* const server);
void update_thread(CServerFramework* server);
void processPacket(CServerFramework* server, CCharacter* pPlayer, char* packet);
void processSCPacket(CServerFramework* server, CCharacter* pPlayer, char* packet);