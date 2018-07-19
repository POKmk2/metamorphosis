#pragma once
#define MAX_BUFF_SIZE 255
#pragma pack(push, 1)
namespace scKIND
{
	enum dataType
	{
		addPlayer, move, stop, rotate, addBullet, addSkill, hitBullet
	};
}

namespace csKIND
{
	enum dataType
	{
		addPlayer, move, stop, rotate, addBullet, addSkill, hitBullet
	};
}

struct cs_packet_move
{
	BYTE size;
	BYTE type;
	DWORD direction;
};

struct sc_packet_move
{
	BYTE size;
	BYTE type;
	DWORD direction;
	float positionX;
	float positionY;
	float positionZ;
	int id;
};

struct cs_packet_addplayer
{
	BYTE size;
	BYTE type;
	BYTE weapon;//무기
	BYTE main1;//스킬종류 1, 11
	BYTE sub11;
	BYTE sub12;
	BYTE main2;
	BYTE sub21;
	BYTE sub22;
	BYTE main3;
	BYTE sub31;
	BYTE sub32;
	float posX;//위치
	float posY;
	float posZ;
	float vectorX;
	float vectorY;
	float vectorZ;
	bool team;
	BYTE hp;
};

struct sc_packet_addplayer
{
	BYTE size;
	BYTE type;
	BYTE weapon;//무기
	BYTE main1;//스킬종류 1, 11
	BYTE sub11;
	BYTE sub12;
	BYTE main2;
	BYTE sub21;
	BYTE sub22;
	BYTE main3;
	BYTE sub31;
	BYTE sub32;
	float posX;//위치
	float posY;
	float posZ;
	float vectorX;
	float vectorY;
	float vectorZ;
	bool team;
	BYTE hp;
	int id;
};

struct sc_packet_rotate
{
	BYTE size;
	BYTE type;
	float posX;
	float posY;
	float posZ;
	float cxdelta;
	float cydelta;
	int id;
};

struct cs_packet_rotate
{
	BYTE size;
	BYTE type;
	float posX;
	float posY;
	float posZ;
	float cxdelta;
	float cydelta;
};

struct cs_packet_addBullet
{
	BYTE size;
	BYTE type;
	int bulletID;
};

struct sc_packet_addBullet
{
	BYTE size;
	BYTE type;
	int bulletID;
	float posX;
	float posY;
	float posZ;
	float vectorX;
	float vectorY;
	float vectorZ;
	bool team;
	int id;
};

struct cs_packet_addSkill
{
	BYTE size;
	BYTE type;
	int skillID;
	int idx;
};

struct sc_packet_addSkill
{
	BYTE size;
	BYTE type;
	float posX;
	float posY;
	float posZ;
	float vectorX;
	float vectorY;
	float vectorZ;
	int skillID;
	int idx;
	int id;
};

struct sc_packet_hitBullet
{
	BYTE size;
	BYTE type;
	int victimID;
	int ownerID;
	int bulletID;
};

//이건 나중에 없어져야 함
struct cs_packet_hitBullet
{
	BYTE size;
	BYTE type;
	int victimID;
	int ownerID;
	int bulletID;
};
#pragma pack(pop)
