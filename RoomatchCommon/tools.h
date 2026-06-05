#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include <QString>

class Tools : public QObject {
    Q_OBJECT
public:
    explicit Tools(QObject *parent = nullptr);
    // 静态加密函数
    static QString encryptPassword(const QString &clearPassword);

signals:
};

struct USERINFO {
    QString username;
    QString pwd;
    int type;
};

// 用于返回一些操作的状态
struct STATUS {
    int code;
    QString info;
};

#endif // TOOLS_H
