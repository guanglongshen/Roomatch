#include "databaseworker.h"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QSqlError>

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
            emit logMessage(tr("账户数据表创建成功！"));
        } else {
            QString error = tr("账户数据表创建失败，详细：") + db.lastError().text();
            emit logMessage(error);
            return ;
        }
    } else {
        QString error = tr("数据库打开失败，详细：") + db.lastError().text();
        emit logMessage(error);
        qDebug() << "Database open failed:" << db.lastError().text();
    }
}
