#pragma once
enum effectType :BYTE
{
	e_damage, e_shield, e_buff, e_dotDamage, e_increaseRange, e_bodyShield,e_createBullet
};

enum SkillList :BYTE
{
	grenade, poison, shield
};

enum addEffectType :BYTE
{
	mainType, subActive, subPassive
};


struct Effect
{
	BYTE m_type;
	float m_dmg;
	float m_range;
	float m_time;
	//필요한 것 추가하기
	Effect(BYTE type, float dmg, float range, float time)
	{
		m_type = type;
		m_dmg = dmg;
		m_range = range;
		m_time = time;
	}
	void update(float time)
	{
		m_time -= time;
	}
	bool isDead()
	{
		if (time < 0)
		{
			return true;
		}
		return false;
	}
};


class CSkillEffect
{
public:
	CSkillEffect();
	~CSkillEffect();
	list<Effect*> m_effect;
	void initiate(BYTE skill, BYTE type);
	bool m_Timing_create = false;
	bool m_Timing_update = false;
	bool m_Timing_takeEffect = false;
	bool m_Timing_bullet = false;
};

