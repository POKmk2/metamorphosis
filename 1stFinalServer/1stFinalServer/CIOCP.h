#pragma once
#include "CRoom.h"
class CIOCP
{
public:
	HANDLE m_kernelbase;

	CIOCP();
	~CIOCP();
	void Initiate();
	void SendPacketToPlayer(CCharacter* p, void*ptr);
	void SendPacketToRoom(CCharacter* p, void*ptr);
};

