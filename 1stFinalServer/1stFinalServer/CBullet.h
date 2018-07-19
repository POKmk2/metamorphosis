#pragma once
#include "CGameObject.h"
class CBullet :public CGameObject
{
public:
	int m_ownerID;
	float m_continueTime;
	float m_damage;
	bool m_team;
	CBullet() = delete;
	CBullet(int bulletID);
	~CBullet();
	virtual void initiate(XMFLOAT4X4 & world, XMFLOAT3 & vector, XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation, float dmg, bool team, int ownerID, float speed = 1.0f);
	virtual void update(float time);
	bool isDead();
};

