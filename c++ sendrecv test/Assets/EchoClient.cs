using System.Collections;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Net;
using System.Runtime.InteropServices;
using System;
using UnityEngine;

public struct cs_packet_move
{
    public byte type;
    public int direction;
};

public struct sc_packet_move
{
    public byte type;
    public int direction;
    public float positionX;
    public float positionY;
    public float positionZ;
    public int id;
};

public struct cs_packet_addplayer
{
    public byte type;
    public byte weapon;//무기
    public byte main1;//스킬종류 1, 11
    public byte sub11;
    public byte sub12;
    public byte main2;
    public byte sub21;
    public byte sub22;
    public byte main3;
    public byte sub31;
    public byte sub32;
    public float posX;//위치
    public float posY;
    public float posZ;
    public float vectorX;
    public float vectorY;
    public float vectorZ;
    public bool team;
    public byte hp;
};

public struct sc_packet_addplayer
{
    public byte type;
    public byte weapon;//무기
    public byte main1;//스킬종류 1, 11
    public byte sub11;
    public byte sub12;
    public byte main2;
    public byte sub21;
    public byte sub22;
    public byte main3;
    public byte sub31;
    public byte sub32;
    public float posX;//위치
    public float posY;
    public float posZ;
    public float vectorX;
    public float vectorY;
    public float vectorZ;
    public bool team;
    public byte hp;
    public int id;
    
};

public struct sc_packet_rotate
{
    public byte type;
    public float posX;
    public float posY;
    public float posZ;
    public float cxdelta;
    public float cydelta;
    public int id;
};

public struct cs_packet_rotate
{
    public byte size;
    public byte type;
    public float posX;
    public float posY;
    public float posZ;
    public float cxdelta;
    public float cydelta;
};

public struct cs_packet_addBullet
{
    public byte size;
    public byte type;
    public int bulletID;
};

public struct sc_packet_addBullet
{
    public byte size;
    public byte type;
    public int bulletID;
    public float posX;
    public float posY;
    public float posZ;
    public float vectorX;
    public float vectorY;
    public float vectorZ;
    public bool team;
    public int id;
};

public struct cs_packet_addSkill
{
   public byte size;
   public byte type;
   public int skillID;
   public int idx;
};

public struct sc_packet_addSkill
{
    public byte size;
    public byte type;
    public float posX;
    public float posY;
    public float posZ;
    public float vectorX;
    public float vectorY;
    public float vectorZ;
    public int skillID;
    public int idx;
    public int id;
};

public struct sc_packet_hitBullet
{
    public byte size;
    public byte type;
    public int victimID;
    public int ownerID;
    public int bulletID;
};

//이건 나중에 없어져야 함
public struct cs_packet_hitBullet
{
    public byte size;
    public byte type;
    public int victimID;
    public int ownerID;
    public int bulletID;
};

public class EchoClient : MonoBehaviour
{
    Socket sock;
    Player[] m_players;
    int m_myID;
    public enum DataType : byte
    {
        addPlayer, move, stop, rotate, addBullet, addSkill, hitBullet
    };
    public static T ByteToStruct<T>(byte[] buffer) where T : struct
    {
        int size = Marshal.SizeOf(typeof(T));
        IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.Copy(buffer, 0, ptr, size);
        T obj = (T)Marshal.PtrToStructure(ptr, typeof(T));
        Marshal.FreeHGlobal(ptr);
        return obj;
    }

    void Start()
    {
        m_players = new Player[8];
        sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        //IP,스트림 소켓, TCP사용
        var ep = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 9000);
        sock.Connect(ep);
        StartCoroutine(Watch());
    }

    IEnumerator Watch()
    {
        sc_packet_addplayer addPlayerPacket;
        sc_packet_move movePacket;
        sc_packet_rotate rotatePacket;
        byte[] receiverBuff = new byte[256];
        byte[] size = new byte[1];
        int playerNumber = 1;
        int target = -1;
        sock.Receive(size, 1, SocketFlags.None);
        sock.Receive(receiverBuff, (int)size[0] - 1, SocketFlags.None);
        addPlayerPacket = ByteToStruct<sc_packet_addplayer>(receiverBuff);
        m_players[0].m_serverPosition[0] = addPlayerPacket.posX;
        m_players[0].m_serverPosition[1] = addPlayerPacket.posY;
        m_players[0].m_serverPosition[2] = addPlayerPacket.posZ;
        m_players[0].m_objectID = addPlayerPacket.id;
        m_players[0].m_team = addPlayerPacket.team;
        //이외의 것들은 클라에서 세팅해야 함.
        while (true)
        {
            sock.Receive(size, 1, SocketFlags.None);
            int result = sock.Receive(receiverBuff, (int)size[0] - 1, SocketFlags.None);
            switch (receiverBuff[0])
            {
                case (byte)DataType.addPlayer:
                    //함수로 뺄 수 있을 듯
                    addPlayerPacket = ByteToStruct<sc_packet_addplayer>(receiverBuff);
                    {
                        m_players[playerNumber].m_serverPosition[0] = addPlayerPacket.posX;
                        m_players[playerNumber].m_serverPosition[1] = addPlayerPacket.posY;
                        m_players[playerNumber].m_serverPosition[2] = addPlayerPacket.posZ;
                        m_players[playerNumber].m_vector[0] = addPlayerPacket.vectorX;
                        m_players[playerNumber].m_vector[1] = addPlayerPacket.vectorY;
                        m_players[playerNumber].m_vector[2] = addPlayerPacket.vectorZ;
                        m_players[playerNumber].m_team = addPlayerPacket.team;
                        m_players[playerNumber].m_objectID = addPlayerPacket.id;
                        m_players[playerNumber].m_skills[0].main = addPlayerPacket.main1;
                        m_players[playerNumber].m_skills[0].sub1 = addPlayerPacket.sub11;
                        m_players[playerNumber].m_skills[0].sub2 = addPlayerPacket.sub12;
                        m_players[playerNumber].m_skills[1].main = addPlayerPacket.main2;
                        m_players[playerNumber].m_skills[1].sub1 = addPlayerPacket.sub21;
                        m_players[playerNumber].m_skills[1].sub2 = addPlayerPacket.sub22;
                        m_players[playerNumber].m_skills[2].main = addPlayerPacket.main3;
                        m_players[playerNumber].m_skills[2].sub1 = addPlayerPacket.sub31;
                        m_players[playerNumber].m_skills[2].sub2 = addPlayerPacket.sub32;
                        m_players[playerNumber].m_maxHP = addPlayerPacket.hp;
                        m_players[playerNumber].m_weaponType = addPlayerPacket.weapon;
                        m_players[playerNumber].GetComponent<MeshRenderer>().enabled = true;
                        playerNumber++;
                    }
                    print("플레이어 추가 패킷 도착");
                    break;
                case (byte)DataType.move:
                    movePacket = ByteToStruct<sc_packet_move>(receiverBuff);
                    for (int i = 0; i < 8; ++i)
                    {
                        if (m_players[i].m_objectID == movePacket.id)
                        {
                            target = i;
                            break;
                        }
                    }
                    m_players[target].m_theWayYouMove = movePacket.direction;
                    m_players[target].m_serverPosition[0] = movePacket.positionX;
                    m_players[target].m_serverPosition[1] = movePacket.positionY;
                    m_players[target].m_serverPosition[2] = movePacket.positionZ;
                    print("플레이어 이동 패킷 도착");
                    break;
                case (byte)DataType.rotate:
                    rotatePacket = ByteToStruct<sc_packet_rotate>(receiverBuff);
                    for (int i = 0; i < 8; ++i)
                    {
                        if (m_players[i].m_objectID == rotatePacket.id)
                        {
                            target = i;
                            break;
                        }
                    }
                    Vector3 angle = (m_players[target].transform.rotation).eulerAngles;
                    angle.x += rotatePacket.cxdelta;
                    angle.y += rotatePacket.cydelta;
                    m_players[target].transform.rotation = Quaternion.Euler(angle);
                    m_players[target].m_serverPosition[0] = rotatePacket.posX;
                    m_players[target].m_serverPosition[1] = rotatePacket.posY;
                    m_players[target].m_serverPosition[2] = rotatePacket.posZ;
                    print("플레이어 회전 패킷 도착");
                    break;
                case (byte)DataType.addBullet:
                    print("플레이어 탄환 추가 패킷 도착");
                    break;
                case (byte)DataType.hitBullet:
                    print("플레이어 피격 패킷 도착");
                    break;
                default:
                    break;
            }
            yield return null;
        }
    }
    // Update is called once per frame
    void Update()
    {

    }
}
