#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>

class Tools : public QObject {
    Q_OBJECT
public:
    explicit Tools(QObject *parent = nullptr);
    // 静态加密函数
    static QString encryptPassword(const QString &clearPassword);

signals:
};

#endif // TOOLS_H
