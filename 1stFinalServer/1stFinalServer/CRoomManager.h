#pragma once
#include "CRoom.h"
class CServerFramework;
class CRoomManager
{
public:
	list<CCharacter*> m_playerList;
	list<CRoom*> m_roomList;
	//CServerFramework* m_server;
	HANDLE* m_kernel;
	CRoomManager();
	~CRoomManager();
	void update(float time);
	bool addPlayer(CCharacter* p);
	bool disconnectPlayer(int characterID);

};

