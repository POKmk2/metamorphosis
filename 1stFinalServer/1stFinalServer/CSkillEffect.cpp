#include "stdafx.h"
#include "CSkillEffect.h"


CSkillEffect::CSkillEffect()
{
}


CSkillEffect::~CSkillEffect()
{
}

void CSkillEffect::initiate(BYTE skill, BYTE type)
{
	Effect* t;
	//여기선 효과만 넣고 타이밍은 밖에서
	switch (skill)
	{
	case grenade:
		switch (type)
		{
		case mainType:
			m_effect.push_back(new Effect(e_damage, 10.0f, 5,0));
			m_Timing_takeEffect = true;
			break;
		case subActive:
			m_effect.push_back(new Effect(e_increaseRange, 0, 3.0f, 0));
			m_Timing_create = true;
			break;
		case subPassive:
			m_effect.push_back(new Effect(e_createBullet, 0, 3.0f, 3.0f));
			m_Timing_takeEffect = true;
			break;
		}
		break;
	case poison:
		switch (type)
		{
		case mainType:
			m_effect.push_back(new Effect(e_dotDamage, 0.5f, 0, 20.0f));
			m_Timing_bullet = true;
			break;
		case subActive:
			m_effect.push_back(new Effect(e_dotDamage, 0.3f, 0, 20.0f));
			m_Timing_takeEffect = true;
			break;
		case subPassive:
			m_effect.push_back(new Effect(e_dotDamage, 0.5f, 0, 20.0f));
			m_Timing_bullet = true;
			break;
		}
		break;
	case shield:
		switch (type)
		{
		case mainType:
			m_effect.push_back(new Effect(e_shield, 20, 5, 10.0f));
			break;
		case subActive:
			m_effect.push_back(new Effect(e_shield, 10, 3, 5.0f));
			m_Timing_takeEffect = true;
			break;
		case subPassive:
			m_effect.push_back(new Effect(e_bodyShield, 20, 0, 0.0f));
			m_Timing_create = true;
			break;
		}
		break;
	}
}
