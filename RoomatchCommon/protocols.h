#ifndef PROTOCOLS_H
#define PROTOCOLS_H

#include <QtTypes>

class Protocols {
public:
    Protocols();
};

// 教师端广播的数据包
struct SERVERBROADCASTINFO {
    char magic[9];          // 暗号 ROOMATCH
    quint16 tcpPort;        // 教师端 TCP 监听的端口
    char serverName[32];    // 教师名
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

#endif // PROTOCOLS_H
