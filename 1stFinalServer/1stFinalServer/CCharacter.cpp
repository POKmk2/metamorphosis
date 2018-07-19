#include "stdafx.h"
#include "CCharacter.h"


CCharacter::CCharacter(int id):CGameObject(id)
{
	
	ZeroMemory(&exover.m_over, sizeof(WSAOVERLAPPED));
	exover.m_wsabuf.buf = exover.m_iobuf;
	exover.m_wsabuf.len = sizeof(exover.m_iobuf);
	exover.messsageType = isrecv;
	m_prev_packet_size = 0;
}


CCharacter::~CCharacter()
{
}

void CCharacter::update(float time)
{
	m_mutex.lock();
	//가지고 있는 총알 리스트 업데이트
	for (auto a = m_bulletList.begin(); a != m_bulletList.end();)
	{
		(*a)->update(time);//충돌체크는 룸에서 따로 해야 함//이동 새로 만들어야 함
		if ((*a)->isDead())
		{
			m_bulletList.erase(a++);
			if (a == m_bulletList.end())
				break;
		}
		else
		{
			++a;
		}
	}
	m_mutex.unlock();
	for (auto a = m_effectList.begin(); a != m_effectList.end();)
	{
		switch ((*a)->m_type)
		{
		case e_damage:
			m_HP -= (*a)->m_dmg;

		}
	}
	
	//x랑 z로 움직일거야
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	float fDistance = time * m_speed;
	m_vector = XMFLOAT3(0, 0, 0);
	if (m_theWayYouMove != 0)
	{
		if (m_theWayYouMove & DIR_FORWARD) m_vector = Vector3::Add(m_vector, m_xmf3Look);
		if (m_theWayYouMove & DIR_BACKWARD)m_vector = Vector3::Subtract(m_vector, m_xmf3Look);
		if (m_theWayYouMove & DIR_RIGHT)
		{
			XMFLOAT3 temp = XMFLOAT3(m_xmf3Look.x*cos(PI / 2.0) - m_xmf3Look.z*sin(PI / 2.0), 0, m_xmf3Look.x*sin(PI / 2) + m_xmf3Look.z*cos(PI / 2));
			m_vector = Vector3::Add(m_vector, temp);
		}
		if (m_theWayYouMove & DIR_LEFT)
		{
			XMFLOAT3 temp = XMFLOAT3(m_xmf3Look.x*cos(PI*3 / 2.0) - m_xmf3Look.z*sin(PI*3 / 2.0), 0, m_xmf3Look.x*sin(PI*3 / 2) + m_xmf3Look.z*cos(PI*3 / 2));
			m_vector = Vector3::Add(m_vector, temp);
		}
		/*if (m_theWayYouMove & DIR_UP)
		if (m_theWayYouMove & DIR_DOWN)*/
	}
	if (!Vector3::IsZero( m_vector))
		Vector3::Normalize(m_vector);
	m_vector = Vector3::ScalarProduct(m_vector, fDistance, false);
	m_xmf4x4World._41 += m_vector.x;
	m_xmf4x4World._42 += m_vector.y;
	m_xmf4x4World._43 += m_vector.z;
	m_collisionBox.Center = XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
}

void CCharacter::initiate(XMFLOAT4X4 & world, XMFLOAT3 & vector, XMFLOAT3 & xmCenter, XMFLOAT3 & xmExtents, 
	XMFLOAT4 & xmOrientation, CRoom * roomNum)
{
	CGameObject::initiate(world, vector, xmCenter, xmExtents, xmOrientation);
	m_room = roomNum;
}

void CCharacter::setSkill(BYTE main1, BYTE sub11, BYTE sub12, 
	BYTE main2, BYTE sub21, BYTE sub22, BYTE main3, BYTE sub31, BYTE sub32)
{
	m_effects[0] = new CSkillEffect();
	m_effects[0]->initiate(main1, mainType);
	if (main1 == grenade || main1 == shield)
	{
		m_effects[0]->initiate(sub11, subActive);
		m_effects[0]->initiate(sub12, subActive);
	}
	else
	{
		m_effects[0]->initiate(sub11, subPassive);
		m_effects[0]->initiate(sub12, subPassive);
	}

	m_effects[1] = new CSkillEffect();
	m_effects[1]->initiate(main2, mainType);
	if (main2 == grenade || main2 == shield)
	{
		m_effects[1]->initiate(sub21, subActive);
		m_effects[1]->initiate(sub22, subActive);
	}
	else
	{
		m_effects[1]->initiate(sub21, subPassive);
		m_effects[1]->initiate(sub22, subPassive);
	}

	m_effects[2] = new CSkillEffect();
	m_effects[2]->initiate(main3, mainType);
	if (main3 == grenade || main3 == shield)
	{
		m_effects[2]->initiate(sub31, subActive);
		m_effects[2]->initiate(sub32, subActive);
	}
	else
	{
		m_effects[2]->initiate(sub31, subPassive);
		m_effects[2]->initiate(sub32, subPassive);
	}

	m_number[0].main = main1;
	m_number[1].main = main2;
	m_number[2].main = main3;
	m_number[0].sub1 = sub11;
	m_number[0].sub2 = sub12;
	m_number[1].sub1 = sub21;
	m_number[1].sub2 = sub22;
	m_number[2].sub1 = sub31;
	m_number[2].sub2 = sub32;


}

void CCharacter::Rotate(float x, float y, float z)
{
	//마우스위치에서 현재 위치를 빼서 노말라이즈
	XMFLOAT3 mousePos = XMFLOAT3(x, y, z);
	XMFLOAT3 currPos = XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	m_xmf3Look = Vector3::Subtract(mousePos, currPos);
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
}

CBullet * CCharacter::addBullet(int id)
{
	float xSize = 2.5f;
	float ySize = 2.5f;
	float zSize = 2.5f;
	//for (int i = 0; i < 3; ++i)
	//{
	//	XMFLOAT3 xmCenter = XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	//	if (true == m_effects[i]->m_Timing_bullet)
	//	{
	//		for (auto a : m_effects[i]->m_effect)
	//		{
	//			//크기 키우기 등..
	//		}
	//	}
	//}
	XMFLOAT3 xmCenter = XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT4 xmOrientation = XMFLOAT4(0, 0, 0, 1);
	XMFLOAT3 xmExtents = XMFLOAT3(xSize, ySize, zSize);
	CBullet* temp = new CBullet(id);
	temp->initiate(m_xmf4x4World, m_xmf3Look, xmCenter, xmExtents, xmOrientation, 2.0f, m_team, m_objectID,200.0f);
	m_mutex.lock();
	m_bulletList.push_back(temp);
	m_mutex.unlock();
	return temp;
}

