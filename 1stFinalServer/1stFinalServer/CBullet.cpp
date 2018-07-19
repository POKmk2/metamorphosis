#include "stdafx.h"
#include "CBullet.h"


CBullet::CBullet(int bulletID) :CGameObject(bulletID)
{

}


CBullet::~CBullet()
{
}

void CBullet::initiate(XMFLOAT4X4 & world, XMFLOAT3 & vector, 
	XMFLOAT3 & xmCenter, XMFLOAT3 & xmExtents, XMFLOAT4 & xmOrientation, 
	float dmg, bool team, int ownerID, float speed)
{
	CGameObject::initiate(world, vector, xmCenter, xmExtents, xmOrientation, speed);
	m_damage = dmg;
	m_team = team;
	m_ownerID = ownerID;
}

void CBullet::update(float time)
{
	CGameObject::update(time);
	m_continueTime -= time;
}

bool CBullet::isDead()
{
	if (m_continueTime < 0)
	{
		return true;
	}
	return false;
}

