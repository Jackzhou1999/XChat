#ifndef SQLMGR_H
#define SQLMGR_H
#include <QString>
#include <QDateTime>
#include <vector>
#include <optional>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMutex>
#include <QMutexLocker>
#include <QQueue>
#include <QDebug>
#include "userdata.h"
#include <deque>

class SqliteConnectionPool {
public:
    static SqliteConnectionPool& getInstance() {
        static SqliteConnectionPool instance;
        return instance;
    }

    // 获取一个数据库连接
    QSqlDatabase getConnection() {
        QMutexLocker locker(&mutex);

        // 如果有空闲连接，直接返回
        if (!connectionPool.isEmpty()) {
            QString connectionName = connectionPool.dequeue();
            return QSqlDatabase::database(connectionName);
        }

        // 如果没有空闲连接，创建一个新连接
        QString connectionName = QString("sqlite_connection_%1").arg(++connectionCounter);
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(databasePath);

        if (!db.open()) {
            qWarning() << "Failed to open database:" << db.lastError().text();
        }

        return db;
    }

    // 释放数据库连接
    void releaseConnection(const QSqlDatabase& db) {
        QMutexLocker locker(&mutex);
        QString connectionName = db.connectionName();
        if (QSqlDatabase::contains(connectionName)) {
            connectionPool.enqueue(connectionName);
        }
    }

    // 初始化数据库
    void initialize(const QString& dbPath) {
        databasePath = dbPath;
        qDebug() << "initial database";
        // 建表
        QSqlDatabase db = getConnection();
        QSqlQuery query(db);

        // 创建 user 表 uid, name, email, pwd, nick, `desc`, sex, icon
        query.exec(R"(
            CREATE TABLE IF NOT EXISTS user (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                uid BIGINT NOT NULL,
                name TEXT NOT NULL,
                nick TEXT,
                email TEXT,
                sex INTEGER DEFAULT 0,
                icon TEXT,
                desc TEXT,
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                UNIQUE (uid)
            );
        )");

        // 创建 friend_relationships 表
        query.exec(R"(
            CREATE TABLE IF NOT EXISTS friend_relationships (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id BIGINT NOT NULL,
                friend_id BIGINT NOT NULL,
                status TINYINT NOT NULL DEFAULT 1,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                UNIQUE (user_id, friend_id)
            );
        )");

        // 创建 friend_requests 表
        query.exec(R"(
            CREATE TABLE IF NOT EXISTS friend_requests (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                sender_id BIGINT NOT NULL,
                receiver_id BIGINT NOT NULL,
                message TEXT,
                status TINYINT NOT NULL DEFAULT 0,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                rejection_reason TEXT,
                is_sender_read BOOLEAN DEFAULT FALSE,
                UNIQUE (sender_id, receiver_id)
            );
        )");

        // 创建 chat_messages 表
        query.exec(R"(
            CREATE TABLE IF NOT EXISTS chat_messages (
                id INTEGER PRIMARY KEY,
                sender_id BIGINT NOT NULL,
                receiver_id BIGINT NOT NULL,
                is_group TINYINT NOT NULL DEFAULT 0,
                content TEXT NOT NULL,
                content_type TINYINT NOT NULL DEFAULT 1,
                status TINYINT NOT NULL DEFAULT 0,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
            );
        )");

        releaseConnection(db);
    }

private:
    SqliteConnectionPool() = default;
    ~SqliteConnectionPool() {
        // 关闭所有数据库连接
        QMutexLocker locker(&mutex);
        while (!connectionPool.isEmpty()) {
            QString connectionName = connectionPool.dequeue();
            QSqlDatabase::removeDatabase(connectionName);
        }
    }

    Q_DISABLE_COPY(SqliteConnectionPool)

    QMutex mutex;
    QQueue<QString> connectionPool;
    QString databasePath;
    int connectionCounter = 0;
};

class DatabaseManager {
public:
    // 查询好友关系表条目
    static FriendRelationship getFriendRelationshipByUid(qint64 user_id, qint64 friend_id);

    // 查询好友的用户信息
    static DbUserInfo getUserInfoByUid(qint64 friend_uid);

    // 分页查询聊天记录
    static std::deque<ChatMessage> getChatMessages(qint64 user_id, qint64 friend_id, int limit, QString& timestamp);

    // 查询与好友的最后一条聊天记录
    static std::optional<ChatMessage> getLastChatMessage(qint64 user_id, qint64 friend_id);

    // 查询某用户发送给本用户的未读消息数量
    static int getUnreadMessageCountFromUser(qint64 uid, qint64 current_user_id, QVector<qint64>& unreadIds);

    // 将信息标记为已读
    static void markMessagesAsRead(const std::vector<qint64>& messageIds);

    // 插入用户信息
    static void insertUser(qint64 uid, const QString& name, const QString& email, const QString& nick,
                    const QString& desc, int sex, const QString& icon);

    // 插入 friend_relationships 表数据
    static void insertFriendRelationship(qint64 user_id, qint64 friend_id, int status, const QString& created_at, const QString& updated_at);

    // 插入 friend_requests 表数据
    static void insertFriendRequest(qint64 sender_id, qint64 receiver_id, const QString& message, int status,
                             const QString& created_at, const QString& updated_at, const QString& rejection_reason,
                             bool is_sender_read);

    // 插入 chat_messages 表数据
    static void insertChatMessage(qint64 id, qint64 sender_id, qint64 receiver_id, bool is_group, const QString& content, int content_type,
                           int status, const QString& created_at, const QString& updated_at);

    // 查询我发送的好友申请
    static std::vector<FriendRequest> getMyFriendRequests(qint64 currentUserId);

    // 查询我受到的好友申请
    static std::vector<FriendRequest> getNewFriendRequests(qint64 currentUserId);

    // 查询所有的朋友uid
    static QVector<qint64> getAllFriendIds();

    static std::vector<DbUserInfo> getUserInfoByUids(const QVector<qint64> &uids);

    // 查询未读信息发送者ids
    static QVector<qint64> getAllUnreadSenderIds(int currentUserId);

    static std::optional<FriendRequest> getFriendRequestBySenderAndReceiver(qint64 senderId, qint64 receiverId);

    static bool setMessageStatusToRead(qint64 messageId);

};

#endif // SQLMGR_H
