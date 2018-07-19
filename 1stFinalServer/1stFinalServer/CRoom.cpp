#include "stdafx.h"
#include "CRoom.h"


CRoom::CRoom()
{
	block[0] = new CGameObject(0);
	XMFLOAT4X4 tempWorld = { 1,0,0,0,0,1,0,0,0,0,1,0,450,0,350,0 };
	XMFLOAT3 tempVector = XMFLOAT3(0, 0, 0);
	XMFLOAT3 tempCenter = XMFLOAT3(450, 0, 350);
	XMFLOAT3 tempExtents = XMFLOAT3(50, 50, 50);
	XMFLOAT4 tempOrientation = XMFLOAT4(0, 0, 0, 1);
	block[0]->initiate(tempWorld, tempVector, tempCenter, tempExtents, tempOrientation, 0.0f);

	block[1] = new CGameObject(0);
	tempWorld = { 1,0,0,0,0,1,0,0,0,0,1,0,750,0,350,0 };
	tempVector = XMFLOAT3(0, 0, 0);
	tempCenter = XMFLOAT3(750, 0, 350);
	tempExtents = XMFLOAT3(50, 50, 50);
	tempOrientation = XMFLOAT4(0, 0, 0, 1);
	block[1]->initiate(tempWorld, tempVector, tempCenter, tempExtents, tempOrientation, 0.0f);

	block[2] = new CGameObject(0);
	tempWorld = { 1,0,0,0,0,1,0,0,0,0,1,0,600,0,550,0 };
	tempVector = XMFLOAT3(0, 0, 0);
	tempCenter = XMFLOAT3(600, 0, 550);
	tempExtents = XMFLOAT3(50, 50, 50);
	tempOrientation = XMFLOAT4(0, 0, 0, 1);
	block[2]->initiate(tempWorld, tempVector, tempCenter, tempExtents, tempOrientation, 0.0f);
}


CRoom::~CRoom()
{
}

void CRoom::update(float time)
{
	sc_packet_hitBullet hitBulletPacket;
	
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			if (m_players[i] != nullptr)
			{
				if (block[j]->collisionCheck(m_players[i]->m_collisionBox))
					m_players[i]->m_xmf3Look = Vector3::ScalarProduct(m_players[i]->m_xmf3Look, -1, false);
				m_players[i]->update(time);

			}
		}
	}



	//불릿 뒤졌는지 여기서 검사함
	for (auto a = m_bullets.begin(); a != m_bullets.end();++a)
	{
		(*a)->update(time);
	}
	for (int j = 0; j < 3; ++j)
	{
		for (auto a = m_bullets.begin(); a != m_bullets.end(); )
		{
			if (block[j]->collisionCheck((*a)->m_collisionBox))
			{
				hitBulletPacket.bulletID = (*a)->m_objectID;
				hitBulletPacket.ownerID = (*a)->m_ownerID;
				hitBulletPacket.victimID = -1;
				hitBulletPacket.size = sizeof(sc_packet_hitBullet);
				hitBulletPacket.type = scKIND::hitBullet;
				for (int i = 0; i < 8; ++i)
				{
					if (m_players[i] == nullptr || m_players[i] == false)
					{
						continue;
					}
					EXOVER *s_over = new EXOVER;
					s_over->messsageType = issend;
					//hitBulletPacket = new sc_packet_hitBullet;
					memcpy(s_over->m_iobuf, &hitBulletPacket, sizeof(sc_packet_hitBullet));
					s_over->m_wsabuf.buf = s_over->m_iobuf;
					s_over->m_wsabuf.len = s_over->m_iobuf[0];
					ZeroMemory(&s_over->m_over, sizeof(WSAOVERLAPPED));
					WSASend(m_players[i]->m_sock, &s_over->m_wsabuf, 1, NULL, 0, &s_over->m_over, NULL);
				}
				m_bullets.erase(a++);
			}
			else
			{
				++a;
			}
		}
	}

	for (int i = 0; i < 8; ++i)
	{
		for (auto a = m_bullets.begin(); a != m_bullets.end();)
		{
			if (m_players[i] == nullptr)
				break;
			if (m_players[i]->m_team == (*a)->m_team)
				++a;
			else if (m_players[i]->collisionCheck((*a)->m_collisionBox))//충돌하면..
			{
				//동시에 부딪힐 때..? 한 총알에 둘이 맞을 수 있나?
				//총알 소멸, 캐릭터 hp감소
				hitBulletPacket.bulletID = (*a)->m_objectID;
				hitBulletPacket.ownerID = (*a)->m_ownerID;
				hitBulletPacket.victimID = m_players[i]->m_objectID;
				hitBulletPacket.size = sizeof(sc_packet_hitBullet);
				hitBulletPacket.type = scKIND::hitBullet;
				//패킷 보내기
				for (int j = 0; j < 8; ++j)
				{
					if (m_players[j] == nullptr || m_players[j] == false)
					{
						continue;
					}
					EXOVER *s_over = new EXOVER;
					s_over->messsageType = issend;
					//hitBulletPacket = new sc_packet_hitBullet;
					memcpy(s_over->m_iobuf, &hitBulletPacket, sizeof(sc_packet_hitBullet));
					s_over->m_wsabuf.buf = s_over->m_iobuf;
					s_over->m_wsabuf.len = s_over->m_iobuf[0];
					ZeroMemory(&s_over->m_over, sizeof(WSAOVERLAPPED));
					WSASend(m_players[j]->m_sock, &s_over->m_wsabuf, 1, NULL, 0, &s_over->m_over, NULL);
				}
				//PostQueuedCompletionStatus(m_kernelbase, sizeof(sc_packet_hitBullet), (ULONG_PTR)&((*a)->m_ownerID), &s_over->m_over);
				//업데이트를 자주 못하게 되면 위에꺼 써야 함
				
				m_players[i]->m_HP -= (*a)->m_damage;
				m_bullets.erase(a++);
				break;
			}
			else
			{
				++a;
			}
		}
	}



	for (int i = 0; i < 8; ++i)
	{
		if (m_players[i]->m_HP < 0)
		{
			//사망 처리
		}
	}
}