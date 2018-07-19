#include "stdafx.h"
#include "CGameObject.h"


CGameObject::CGameObject(int objectID)
{
	m_objectID = objectID;
}

CGameObject::~CGameObject()
{
}

void CGameObject::initiate(XMFLOAT4X4 & world, XMFLOAT3 & vector, XMFLOAT3& xmCenter, XMFLOAT3& xmExtents, XMFLOAT4& xmOrientation, float speed)
{
	m_xmf4x4World = world;
	m_vector = vector;
	m_collisionBox = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation);
	m_speed = speed;
}

void CGameObject::update(float time)
{
	XMVECTOR temp = XMLoadFloat3(&m_vector);
	temp = XMVector3Normalize(temp);
	XMStoreFloat3(&m_vector, temp);
	m_xmf4x4World._41 += m_vector.x*time*m_speed;
	m_xmf4x4World._42 += m_vector.y*time*m_speed;
	m_xmf4x4World._43 += m_vector.z*time*m_speed;
	XMFLOAT3 center = XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	m_collisionBox.Center = center;
	//OOBB돌리는 부분 없음.
}

bool CGameObject::collisionCheck(const BoundingOrientedBox& other)
{
	//return m_collisionBox.Intersects(other);
	return ((this->m_xmf4x4World._41 - other.Center.x)*(this->m_xmf4x4World._41 - other.Center.x)
		+ (this->m_xmf4x4World._43 - other.Center.z)*(this->m_xmf4x4World._43 - other.Center.z)
		< (m_collisionBox.Extents.x + other.Extents.x) *(m_collisionBox.Extents.x + other.Extents.x));
}