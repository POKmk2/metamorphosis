#pragma once
#include "CBullet.h"
#include "CSkillEffect.h"
#define PI 3.14
struct SKILLNUMBER
{
	BYTE main;
	BYTE sub1;
	BYTE sub2;
};

class CRoom;

class CCharacter : public CGameObject
{
public:
	CRoom * m_room;
	bool			m_team;
	int				m_HP=10;
	short			m_maxHP;
	SKILLNUMBER		m_number[3];
	CSkillEffect*	m_effects[3];
	list<CBullet*>  m_bulletList;
	list<CBullet*>  m_skillList;
	list<Effect*>	m_effectList;
	DWORD			m_theWayYouMove;
	
	mutex m_mutex;

	SOCKET m_sock;
	bool m_isconnected = false;
	EXOVER exover;
	int m_packet_size;  // 지금 조립하고 있는 패킷의 크기
	int	m_prev_packet_size; // 지난번 recv에서 완성되지 않아서 저장해 놓은 패킷의 앞부분의 크기
	char m_packet[MAX_PACKET_SIZE];

	CCharacter(int id);
	~CCharacter();

	XMFLOAT3		m_xmf3Look = XMFLOAT3(1.0f, 0.0f, 0.0f);
	

	void update(float time);
	void initiate(XMFLOAT4X4& world, XMFLOAT3& vector, XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation, CRoom* roomNum);
	void setSkill(BYTE main1, BYTE sub11, BYTE sub12, BYTE main2, BYTE sub21, BYTE sub22, BYTE main3, BYTE sub31, BYTE sub32);
	void Rotate(float x, float y, float z);
	CBullet* addBullet(int id);
};

