#include "databaseworker.h"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QSqlError>
#include <tools.h>

DatabaseWorker::DatabaseWorker(QObject *parent)
    : QObject{parent} {}

void DatabaseWorker::onInitializeDatabase() {
    // 创建数据库并设置可写路径，用户 / AppData / Roomatch / Roomatch Server
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "database_worker");
    QString dbDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dbDir);

    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString dbPath = dir.filePath("roomatch.db");
    db.setDatabaseName(dbPath);

    if (db.open()) {
        // 创建若干初始表
        QSqlQuery query(db);
        QString createTableSql = R"(
            CREATE TABLE IF NOT EXISTS account (
                id INTEGER PRIMARY KEY AUTOINCREMENT,   -- id 默认自增长
                username TEXT NOT NULL UNIQUE,          -- 用户名：不允许为空，且必须唯一
                password TEXT NOT NULL,                 -- 密码：不允许为空
                type INTEGER NOT NULL,                  -- 用户类型：0 代表教师，1 代表学生
                last_login DATETIME,                    -- 记录最后一次登录时间
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP -- 账号创建时间
            )
        )";

        if (query.exec(createTableSql)) {
            emit logMessage(tr("account 数据表创建"), "ok", "Database");
        } else {
            emit logMessage(tr("account 数据表创建"), db.lastError().text(), "Database");
            return ;
        }
    } else {
        emit logMessage(tr("数据库"), db.lastError().text(), "Database");
    }
}

void DatabaseWorker::onRegisterUser(const USERINFO &registerInfo) {
    QSqlDatabase db = QSqlDatabase::database("database_worker");

    if (!db.isOpen()) {
        emit logMessage(tr("数据库"), db.lastError().text(), "Database");
        return ;
    }

    // 先检测 教师/学生 它的 username 是否唯一
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM account WHERE username = :username AND type = :type");
    query.bindValue(":username", registerInfo.username);
    query.bindValue(":type", registerInfo.type);

    if (!query.exec() || !query.next()) {
        emit logMessage(tr("用户查询"), db.lastError().text(), "Database");
        return ;
    }

    // 存在同名用户，发出信号并停止插入记录
    if (query.value(0).toInt() > 0) {
        STATUS status = { 1, tr("注册失败，已存在同名用户！") };
        emit registerUserResponse(status);
        emit logMessage(tr("注册教师用户"), status.info, "Database");
        return ;
    }

    query.prepare("INSERT INTO account (username, password, type) VALUES (:username, :password, :type)");
    query.bindValue(":username", registerInfo.username);
    query.bindValue(":password", registerInfo.pwd);
    query.bindValue(":type", registerInfo.type);

    if (!query.exec()) {
        emit logMessage(tr("注册教师用户"), db.lastError().text(), "Database");
        return ;
    }

    STATUS ok = { 0, tr("成功注册用户: %1-教师。").arg(registerInfo.username) };
    emit logMessage(tr("注册教师用户"), "ok", "Database");
    emit registerUserResponse(ok);
}
