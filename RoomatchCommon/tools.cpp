#include "tools.h"
#include <QByteArray>
#include <QCryptographicHash>

Tools::Tools(QObject *parent)
    : QObject{parent} {}

QString Tools::encryptPassword(const QString &clearPassword) {
    // 盐值，再加密
    QString salt = "@Roomatch_CS_encrypt2026";
    QString saltedPassword = clearPassword + salt;

    // 转换为 QByteArray
    QByteArray passwordBytes = saltedPassword.toUtf8();

    // 进行 SHA-256 单项哈希计算
    QByteArray hashBytes = QCryptographicHash::hash(passwordBytes, QCryptographicHash::Sha256);

    // 转为 16 进制，固定 64 位的字符串
    return hashBytes.toHex();
}
