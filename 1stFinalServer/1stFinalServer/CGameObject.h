#pragma once
class CGameObject
{
public:
	int m_objectID;

	XMFLOAT4X4						m_xmf4x4World;
	XMFLOAT3						m_vector;
	BoundingOrientedBox				m_collisionBox;
	float							m_speed = 30.0f;
	CGameObject() = delete;
	~CGameObject();
	CGameObject(int objectID);

	virtual void initiate(XMFLOAT4X4 & world, XMFLOAT3 & vector, XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation, float speed=30.0f);
	virtual void update(float time);
	virtual bool collisionCheck(const BoundingOrientedBox& other);
};

