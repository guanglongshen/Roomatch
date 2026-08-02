#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include <QString>
#include <QSettings>
#include <QCoreApplication>

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

// 配置系统
class AppConfig {
public:
    static QSettings &settings() {
        static QSettings instance(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
        return instance;
    }
};

struct PROBLEMDETAIL {
    // 题目提供者不需要在保存前调取，登录的时候数据库已经验证，在那个地方记录了谁登录进来，用它的ID即可
    QString title;
    int difficulty, time_limit, memory_limit, type_id;
    QString description, input_format, output_format, samples, hint, testcase_path;
};

#endif // TOOLS_H
