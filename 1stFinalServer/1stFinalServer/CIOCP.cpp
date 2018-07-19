#include "stdafx.h"
#include "CIOCP.h"


CIOCP::CIOCP()
{
}


CIOCP::~CIOCP()
{
}

void CIOCP::Initiate()
{
	m_kernelbase = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
}

void CIOCP::SendPacketToPlayer(CCharacter * p, void * ptr)
{
	char *packet = reinterpret_cast<char *>(ptr);

	EXOVER *s_over = new EXOVER;
	s_over->messsageType = issend;

	memcpy(s_over->m_iobuf, packet, packet[0]);

	s_over->m_wsabuf.buf = s_over->m_iobuf;
	s_over->m_wsabuf.len = s_over->m_iobuf[0];

	ZeroMemory(&s_over->m_over, sizeof(WSAOVERLAPPED));
	int res = WSASend(p->m_sock, &s_over->m_wsabuf, 1, NULL, 0, &s_over->m_over, NULL);
	if (0 != res) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) error_display("Send Error! ", err_no);
	}
}

void CIOCP::SendPacketToRoom(CCharacter * p, void * ptr)
{
	for (int i = 0; i < 8; ++i)
	{
		if (p->m_room->m_players[i] == nullptr || p->m_room->m_players[i]->m_isconnected == false)
		{
			continue;
		}
		SendPacketToPlayer(p->m_room->m_players[i], ptr);
	}
}
