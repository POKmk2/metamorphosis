using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public struct SKILLNUMBER
{
    public byte main;
    public byte sub1;
    public byte sub2;
};

public struct Effect
{
    public byte m_type;
    public float m_dmg;
    public float m_range;
    public float m_time;
    //필요한 것 추가하기
    public Effect(byte type, float dmg, float range, float time)
    {
        m_type = type;
        m_dmg = dmg;
        m_range = range;
        m_time = time;
    }
    public void update(float time)
    {
        m_time -= time;
    }
    public bool isDead()
    {
        if (m_time < 0)
        {
            return true;
        }
        return false;
    }
};

public class CSkillEffect
{
    
    public List<Effect> m_effects;
    public bool m_Timing_create = false;
    public bool m_Timing_update = false;
    public bool m_Timing_takeEffect = false;
    public bool m_Timing_bullet = false;
	public CSkillEffect()
    {

    }
    ~CSkillEffect()
    {

    }
    public void AddEffect (byte skill, byte type)
    {
        //스위치 케이스
        //m_effects.Add(new Effect())
    }
};

public class Player : MonoBehaviour {

    //id
    public int m_objectID;

    //이동
    public int m_theWayYouMove;
    public float[] m_vector = new float[3];
    public float m_speed = 30.0f;
    public float[] m_serverPosition = new float[3];
    
    //무기 및 스킬
    public SKILLNUMBER[] m_skills = new SKILLNUMBER[3];
    public int m_weaponType;

    //죽음 처리
    public int m_HP = 10;
    public int m_maxHP;

    //그리기 및 피격 처리
    public bool m_team;

    //탄환 및 스킬
    public List<GameObject> m_bullets;
    public List<GameObject> m_skillProjectiles;
    public List<Effect> m_effects;

    // Use this for initialization
    void Start () {
        m_vector = new float[3];
	}
	
	// Update is called once per frame
	void Update () {
		
	}

    
}
