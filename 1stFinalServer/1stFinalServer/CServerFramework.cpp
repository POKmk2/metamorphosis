#include "stdafx.h"
#include "CServerFramework.h"


CServerFramework::CServerFramework()
{
	m_start = std::chrono::system_clock::now();
}


CServerFramework::~CServerFramework()
{
}

void CServerFramework::update()
{
	std::chrono::duration<float> sec = std::chrono::system_clock::now() - m_start;
	m_roomManager.update(sec.count());
	m_start = std::chrono::system_clock::now();
}

void CServerFramework::initiate()
{
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	m_IOCP.Initiate();
	m_roomManager.m_kernel = &m_IOCP.m_kernelbase;
	for (int i = 0; i < 4; ++i)
	{
		new thread(worker_thread, this);
	}

	new thread(update_thread, this);
}

void accept_thread(CServerFramework* server)
{
	static int id = 0;
	SOCKET s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	/*ErrorDisplay("In Accept Thread: WSASocket()");
	*/
	SOCKADDR_IN bind_addr;
	ZeroMemory(&bind_addr, sizeof(SOCKADDR_IN));
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_port = htons(MY_SERVER_PORT);
	bind_addr.sin_addr.s_addr = INADDR_ANY;

	::bind(s, reinterpret_cast<sockaddr *>(&bind_addr), sizeof(bind_addr));
	listen(s, 1000);

	while (true)
	{
		SOCKADDR_IN c_addr;
		ZeroMemory(&c_addr, sizeof(SOCKADDR_IN));
		c_addr.sin_family = AF_INET;
		c_addr.sin_port = htons(MY_SERVER_PORT);
		c_addr.sin_addr.s_addr = INADDR_ANY;	// 0.0.0.0  아무대서나 오는 것을 다 받겠다.
		int addr_size = sizeof(sockaddr);

		SOCKET cs = WSAAccept(s, reinterpret_cast<sockaddr *>(&c_addr), &addr_size, NULL, NULL);
		if (INVALID_SOCKET == cs) {
			ErrorDisplay("In Accept Thread:WSAAccept()");
			continue;
		}
		 cout << "New Client Connected!\n";
		CCharacter* p = new CCharacter(id++);
		p->m_sock = cs;
		server->m_roomManager.addPlayer(p);
		//방 배정
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(cs), server->m_IOCP.m_kernelbase, p->m_objectID, 0);
		p->m_isconnected = true;
		unsigned long r_flag = 0;
		ZeroMemory(&p->exover.m_over, sizeof(WSAOVERLAPPED));
		int ret = WSARecv(p->m_sock, &p->exover.m_wsabuf, 1, NULL, &r_flag, &p->exover.m_over, NULL);
		if (0 != ret) {
			int err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no) error_display("Recv Error", err_no);
		}
		//클라에서 데이터 받아서 처리해야 하므로 여기까지.
		//accept에서 다른 플레이어 정보는 다 보내놔도 될 듯 추가하자.
		//accept에서 내 id정도는 받아 놔야 겠군
		sc_packet_addplayer add;
		CCharacter* other;
		//클라에서 recv한 후 send하도록 해야 할 듯 초기 정보
		add.id = p->m_objectID;
		add.size = sizeof(sc_packet_addplayer);
		add.type = scKIND::addPlayer;
		p->m_xmf4x4World._41 = add.posX = 100 + add.id * 100;
		p->m_xmf4x4World._42 = add.posY = 0;
		p->m_xmf4x4World._43 = add.posZ = 300;
		
		add.team = p->m_team;
		//자기 자신의 id를 보내는 행위
		server->m_IOCP.SendPacketToPlayer(p, &add);

		for (int i = 0; i < ROOM_MAX_USER; ++i)
		{
			other = p->m_room->m_players[i];
			if (other == nullptr || other->m_isconnected == false || other == p)
				continue;
			add.main1 = other->m_number[0].main;
			add.main2 = other->m_number[1].main;
			add.main3 = other->m_number[2].main;

			add.sub11 = other->m_number[0].sub1;
			add.sub21 = other->m_number[1].sub1;
			add.sub31 = other->m_number[2].sub1;

			add.sub12 = other->m_number[0].sub2;
			add.sub22 = other->m_number[1].sub2;
			add.sub32 = other->m_number[2].sub2;

			add.posX = other->m_xmf4x4World._41;
			add.posY = other->m_xmf4x4World._42;
			add.posZ = other->m_xmf4x4World._43;

			add.vectorX = other->m_vector.x;
			add.vectorY = other->m_vector.y;
			add.vectorZ = other->m_vector.z;

			add.team = other->m_team;
			add.id = other->m_objectID;
			add.size = sizeof(sc_packet_addplayer);
			add.type = scKIND::addPlayer;
			server->m_IOCP.SendPacketToPlayer(p, &add);
		}
	}
}

void worker_thread(CServerFramework* const server)
{
	while (true)
	{
		unsigned long io_size;
		unsigned long long iocp_key; // 64 비트 integer , 우리가 64비트로 컴파일해서 64비트
		WSAOVERLAPPED *over;
		BOOL ret = GetQueuedCompletionStatus(server->m_IOCP.m_kernelbase, &io_size, &iocp_key, &over, INFINITE);
		int key = static_cast<int>(iocp_key);
		// cout << "WT::Network I/O with Client [" << key << "]\n";
		if (FALSE == ret) {
			ErrorDisplay("GQCS return");
			server->m_roomManager.disconnectPlayer(key);
			continue;
		}
		if (0 == io_size) {
			//ErrorDisplay("disconnected");
			server->m_roomManager.disconnectPlayer(key);
			continue;
		}
		EXOVER *p_over = reinterpret_cast<EXOVER *>(over);
		if (isrecv == p_over->messsageType)
			//온 게 받은 패킷이라면 처리하고 필요한 내용 송신
		{
			// cout << "WT:Packet From Client [" << key << "]\n";
			int work_size = io_size;
			char *wptr = p_over->m_iobuf;
			CCharacter* pPlayer = nullptr;
			for (auto a : server->m_roomManager.m_playerList)
			{
				if (a->m_objectID == key)
				{
					pPlayer = a;
					break;
				}
			}
			if (pPlayer == nullptr)
				ErrorDisplay("recv, there's no player matching");
			while (0 < work_size)
			{
				int p_size;
				if (0 != pPlayer->m_packet_size)
					p_size = pPlayer->m_packet_size;
				else {
					p_size = wptr[0];
					pPlayer->m_packet_size = p_size;
				}
				int need_size = p_size - pPlayer->m_prev_packet_size;
				if (need_size <= work_size) {
					memcpy(pPlayer->m_packet
						+ pPlayer->m_prev_packet_size, wptr, need_size);

					processPacket(server, pPlayer, pPlayer->m_packet);

					pPlayer->m_prev_packet_size = 0;
					pPlayer->m_packet_size = 0;
					work_size -= need_size;
					wptr += need_size;
				}
				else {
					memcpy(pPlayer->m_packet + pPlayer->m_prev_packet_size, wptr, work_size);
					pPlayer->m_prev_packet_size += work_size;
					work_size = -work_size;
					wptr += work_size;
				}
			}
			unsigned long r_flag = 0;
			ZeroMemory(&pPlayer->exover.m_over, sizeof(WSAOVERLAPPED));
			int ret = WSARecv(pPlayer->m_sock, &pPlayer->exover.m_wsabuf, 1,
				NULL, &r_flag, &pPlayer->exover.m_over, NULL);
			if (0 != ret)
			{
				int err_no = WSAGetLastError();
				if (WSA_IO_PENDING != err_no)
					error_display("Recv Error", err_no);
			}
		}
		else if (issend == p_over->messsageType)
		{  // Send 후처리
		   // cout << "WT:A packet was sent to Client[" << key << "]\n";
		   //// cout << "패킷 전송 완료" << endl;
			delete p_over;
		}
		else
		{
			int work_size = io_size;
			char *wptr = p_over->m_iobuf;
			CCharacter* pPlayer = nullptr;
			for (auto a : server->m_roomManager.m_playerList)
			{
				if (a->m_objectID == key)
				{
					pPlayer = a;
					break;
				}
			}
			if (pPlayer == nullptr)
				ErrorDisplay("update, there's no player matching");
			while (0 < work_size)
			{
				int p_size;
				if (0 != pPlayer->m_packet_size)
					p_size = pPlayer->m_packet_size;
				else {
					p_size = wptr[0];
					pPlayer->m_packet_size = p_size;
				}
				int need_size = p_size - pPlayer->m_prev_packet_size;
				if (need_size <= work_size) {
					memcpy(pPlayer->m_packet
						+ pPlayer->m_prev_packet_size, wptr, need_size);

					processSCPacket(server, pPlayer, pPlayer->m_packet);

					pPlayer->m_prev_packet_size = 0;
					pPlayer->m_packet_size = 0;
					work_size -= need_size;
					wptr += need_size;
				}
				else {
					memcpy(pPlayer->m_packet + pPlayer->m_prev_packet_size, wptr, work_size);
					pPlayer->m_prev_packet_size += work_size;
					work_size = -work_size;
					wptr += work_size;
				}
			}


		}
	}
}

void processPacket(CServerFramework * server, CCharacter * pPlayer, char * packet)
{
	sc_packet_move movePacket;
	sc_packet_addplayer addPacket;
	sc_packet_rotate rotatePacket;
	sc_packet_addBullet addBulletPacket;
	sc_packet_addSkill addSkillPacket;
	
	CBullet* pBullet;
	float xSize = 2.5f;
	float ySize = 2.5f;
	float zSize = 2.5f;
	switch (packet[1])
	{
	case csKIND::move:
		memcpy(&movePacket, packet, sizeof(cs_packet_move));
		if (pPlayer->m_theWayYouMove == movePacket.direction)
			break;
		pPlayer->m_theWayYouMove = movePacket.direction;
		movePacket.positionX = pPlayer->m_xmf4x4World._41;
		movePacket.positionY = pPlayer->m_xmf4x4World._42;
		movePacket.positionZ = pPlayer->m_xmf4x4World._43;
		movePacket.id = pPlayer->m_objectID;
		movePacket.size = sizeof(sc_packet_move);
		movePacket.type = scKIND::move;
		server->m_IOCP.SendPacketToRoom(pPlayer, &movePacket);
		break;
	case csKIND::rotate:
		memcpy(&rotatePacket, packet, sizeof(cs_packet_rotate));
		pPlayer->Rotate(rotatePacket.posX, 0, rotatePacket.posZ);
		rotatePacket.cxdelta = pPlayer->m_xmf3Look.x;
		rotatePacket.cydelta = pPlayer->m_xmf3Look.z;
		rotatePacket.posX = pPlayer->m_xmf4x4World._41;
		rotatePacket.posY = pPlayer->m_xmf4x4World._42;
		rotatePacket.posZ = pPlayer->m_xmf4x4World._43;
		rotatePacket.id = pPlayer->m_objectID;
		rotatePacket.size = sizeof(sc_packet_rotate);
		rotatePacket.type = scKIND::rotate;
		server->m_IOCP.SendPacketToRoom(pPlayer, &rotatePacket);
		break;
	case csKIND::addBullet:
		printf("addBullet\n");
		memcpy(&addBulletPacket, packet, sizeof(cs_packet_addBullet));
		//서버에 bullet추가하는 내용 필요
		pBullet = pPlayer->addBullet(addBulletPacket.bulletID);
		pPlayer->m_room->m_bullets.push_back(pBullet);
		addBulletPacket.posX = pBullet->m_xmf4x4World._41;
		addBulletPacket.posY = pBullet->m_xmf4x4World._42;
		addBulletPacket.posZ = pBullet->m_xmf4x4World._43;
		addBulletPacket.vectorX = pBullet->m_vector.x;
		addBulletPacket.vectorY = pBullet->m_vector.y;
		addBulletPacket.vectorZ = pBullet->m_vector.z;
		addBulletPacket.id = pPlayer->m_objectID;
		addBulletPacket.size = sizeof(sc_packet_addBullet);
		addBulletPacket.type = scKIND::addBullet;
		
		server->m_IOCP.SendPacketToRoom(pPlayer, &addBulletPacket);
		break;
	case csKIND::addSkill:
		memcpy(&addSkillPacket, packet, sizeof(cs_packet_addSkill));
		addSkillPacket.id = pPlayer->m_objectID;
		addSkillPacket.size = sizeof(sc_packet_addSkill);
		addSkillPacket.type = scKIND::addSkill;
		server->m_IOCP.SendPacketToRoom(pPlayer, &addSkillPacket);
		break;
	
	case csKIND::addPlayer:
		//처음 접속할 때 보내는 패킷, 정보를 다른 사람들에게 전달해야 한다.
		// cout << "플레이어 접속 패킷 도착" << endl;
		//회전 정보도 전송할 것..
		memcpy(&addPacket, packet, sizeof(cs_packet_addplayer));
		pPlayer->m_number[0].main = addPacket.main1;
		pPlayer->m_number[0].sub1 = addPacket.sub11;
		pPlayer->m_number[0].sub2 = addPacket.sub12;
		pPlayer->m_number[1].main = addPacket.main2;
		pPlayer->m_number[1].sub1 = addPacket.sub21;
		pPlayer->m_number[1].sub2 = addPacket.sub22;
		pPlayer->m_number[2].main = addPacket.main3;
		pPlayer->m_number[2].sub1 = addPacket.sub31;
		pPlayer->m_number[2].sub2 = addPacket.sub32;
		pPlayer->m_effects[0] = new CSkillEffect();
		pPlayer->m_effects[0]->initiate(addPacket.main1, mainType);
		if (addPacket.main1 == grenade || addPacket.main1 == shield)
		{
			pPlayer->m_effects[0]->initiate(addPacket.sub11, subActive);
			pPlayer->m_effects[0]->initiate(addPacket.sub12, subActive);
		}
		else
		{
			pPlayer->m_effects[0]->initiate(addPacket.sub11, subPassive);
			pPlayer->m_effects[0]->initiate(addPacket.sub12, subPassive);
		}
		pPlayer->m_effects[1] = new CSkillEffect();
		pPlayer->m_effects[1]->initiate(addPacket.main1, mainType);
		if (addPacket.main2 == grenade || addPacket.main2 == shield)
		{
			pPlayer->m_effects[1]->initiate(addPacket.sub21, subActive);
			pPlayer->m_effects[1]->initiate(addPacket.sub22, subActive);
		}
		else
		{
			pPlayer->m_effects[1]->initiate(addPacket.sub21, subPassive);
			pPlayer->m_effects[1]->initiate(addPacket.sub22, subPassive);
		}
		pPlayer->m_effects[2] = new CSkillEffect();
		pPlayer->m_effects[2]->initiate(addPacket.main3, mainType);
		if (addPacket.main3 == grenade || addPacket.main3 == shield)
		{
			pPlayer->m_effects[2]->initiate(addPacket.sub31, subActive);
			pPlayer->m_effects[2]->initiate(addPacket.sub32, subActive);
		}
		else
		{
			pPlayer->m_effects[2]->initiate(addPacket.sub31, subPassive);
			pPlayer->m_effects[2]->initiate(addPacket.sub32, subPassive);
		}
		pPlayer->m_xmf4x4World._41 = addPacket.posX;
		pPlayer->m_xmf4x4World._42 = addPacket.posY;
		pPlayer->m_xmf4x4World._43 = addPacket.posZ;
		pPlayer->m_vector.x = 0;
		pPlayer->m_vector.y = 0;
		pPlayer->m_vector.z = 0;
		pPlayer->m_speed = 30.0;
		XMFLOAT3 xmCenter = XMFLOAT3(pPlayer->m_xmf4x4World._41, pPlayer->m_xmf4x4World._42, pPlayer->m_xmf4x4World._43);
		XMFLOAT3 xmExtent = XMFLOAT3(40, 40, 40);
		XMFLOAT4 xmOrientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		pPlayer->initiate(pPlayer->m_xmf4x4World, pPlayer->m_vector, xmCenter, xmExtent, xmOrientation, pPlayer->m_room);
		addPacket.team = pPlayer->m_team;
		addPacket.size = sizeof(sc_packet_addplayer);
		addPacket.type = scKIND::addPlayer;
		addPacket.id = pPlayer->m_objectID;
		//정보 넣기
		//이제 다 보내야 함
		server->m_IOCP.SendPacketToRoom(pPlayer, &addPacket);
		printf("클라에 보냄");
		break;
	}
}

void processSCPacket(CServerFramework * server, CCharacter * pPlayer, char * packet)
{
	sc_packet_hitBullet hitBulletPacket;
	switch (packet[1])
	{
	case scKIND::hitBullet:
		server->m_IOCP.SendPacketToRoom(pPlayer, packet);
		break;
	}
}

void update_thread(CServerFramework * server)
{
	std::chrono::duration<float> sec;
	while (true)
	{
		for (auto a : server->m_roomManager.m_roomList)
		{
			sec = std::chrono::system_clock::now() - server->m_start;
			a->update(sec.count());
			server->m_start = std::chrono::system_clock::now();
		}
		Sleep(10);
	}
}
