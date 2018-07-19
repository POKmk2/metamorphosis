#include "stdafx.h"
#include "CRoomManager.h"


CRoomManager::CRoomManager()
{
}


CRoomManager::~CRoomManager()
{
}

void CRoomManager::update(float time)
{
	for (auto a : m_roomList)
	{
		a->update(time);
	}
}

bool CRoomManager::addPlayer(CCharacter * p)
{
	m_playerList.push_back(p);
	//게임중이 아닌 빈 방을 찾는다.
	//플레이어를 넣는다.
	for (auto a : m_roomList)
	{
		if (a->m_isPlaying == false)
		{
			for (int i = 0; i < 8; ++i)
			{
				if (a->m_players[i] == nullptr || a->m_players[i]->m_isconnected == false)
				{
					p->m_room = &(*a);
					p->m_isconnected = true;
					a->m_players[i] = p;
					if (a->m_1players < a->m_2players)
					{
						a->m_1players++;
						p->m_team = true;
					}
					else
					{
						a->m_2players++;
						p->m_team = false;
					}
					a->m_numberOfPlayers++;
					return true;
				}
			}
		}
	}
	//없으면 새로 방을 만들어서 추가한다.

	CRoom* rp = new CRoom();
	rp->m_kernelbase = m_kernel;
	m_roomList.push_back(rp);
	for (int i = 0; i < 8; ++i)
	{
		if (rp->m_players[i] == nullptr || rp->m_players[i]->m_isconnected == false)
		{
			p->m_room = &(*rp);
			p->m_isconnected = true;
			rp->m_players[i] = p;

			if (rp->m_1players < rp->m_2players)
			{
				rp->m_1players++;
				p->m_team = true;
			}
			else
			{
				rp->m_2players++;
				p->m_team = false;
			}
			rp->m_numberOfPlayers++;
			return true;
		}

	}
	return false;
}

bool CRoomManager::disconnectPlayer(int characterID)
{
	return false;
}
