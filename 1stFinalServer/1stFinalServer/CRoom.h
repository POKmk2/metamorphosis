#pragma once
#include "CCharacter.h"
class CRoomManager;
class CRoom
{
public:
	CCharacter * m_players[8];
	list<CBullet*> m_bullets;
	CGameObject* block[3];
	bool m_isPlaying;
	int m_numberOfPlayers;
	int m_1players;
	int m_2players;
	HANDLE* m_kernelbase;
	CRoom();
	~CRoom();
	void update(float time);
};