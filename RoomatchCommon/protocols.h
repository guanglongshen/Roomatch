#pragma pack(push, 1) // 开启 1 字节对齐，绝不允许编译器偷偷塞垃圾字节！
#ifndef PROTOCOLS_H
#define PROTOCOLS_H


#include <QtTypes>

class Protocols {
public:
    Protocols();
};

// 广播分 2 个状态，教师登录/教师离线
enum BROADCAST_TYPE :quint16 {
    BROADCAST_ONLINE = 1,
    BROADCAST_OFFLINE = 2
};

// 教师端广播的数据包
struct SERVERBROADCASTINFO {
    char magic[9];          // 暗号 ROOMATCH
    char serverName[32];    // 教师名
    quint16 tcpPort;        // 教师端 TCP 监听的端口
    quint16 state;          // 教师状态
};

/*
 * 工业级通信的标准解法是：固定包头 + 可变包体。
 * 每次发消息，先发一个固定大小的“包头”，告诉对方这个包是干嘛的（消息类型）、后面跟着的数据有多长（数据长度）。
 * */
// 消息类型枚举，区分这个包是干嘛的
enum MESSAGETYPE : quint16 {
    MSG_REGISTER_REQ = 0x1001,      // C->S 注册请求
    MSG_REGISTER_RES = 0x1002,      // S->C 注册回应
    MSG_LOGIN_REQ    = 0x1003,      // C->S 登录请求
    MSG_LOGIN_RES    = 0x1004,      // S->C 登录回应
};

// 统一 TCP 报文包头
struct PACKETHEADER {
    quint32 magic;                  // 协议魔数，固定某值 0x4D544348
    quint16 type;                   // 消息类型，from MESSAGETYPE
    quint32 length;                 // 包体数据的实际长度（字节数）
};

const quint32 MAGICNUM = 0x4D544348;
const quint16 studentListenerPort = 55521;  // 学生监听 UDP 端口
const quint16 teacherListenerPort = 55520;  // 老师监听 UDP 端口

// 网络传输中的用户
struct USERINFO_PACKET {
    char username[32]; // 固定 32 字节大小的格子
    char password[32]; // 固定 32 字节大小的格子
    quint16 type;      // 身份类型
};

#endif // PROTOCOLS_H
#pragma pack(pop)