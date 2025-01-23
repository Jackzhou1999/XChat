#include "sqlmgr.h"


FriendRelationship DatabaseManager::getFriendRelationshipByUid(qint64 user_id, qint64 friend_id) {
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    FriendRelationship relationship = {};
    query.prepare(R"(
            SELECT * FROM friend_relationships
            WHERE user_id = :user_id AND friend_id = :friend_id
        )");
    query.bindValue(":user_id", user_id);
    query.bindValue(":friend_id", friend_id);

    if (query.exec() && query.next()) {
        relationship.id = query.value("id").toInt();
        relationship.user_id = query.value("user_id").toLongLong();
        relationship.friend_id = query.value("friend_id").toLongLong();
        relationship.status = query.value("status").toInt();
        relationship.created_at = query.value("created_at").toDateTime();
        relationship.updated_at = query.value("updated_at").toDateTime();
    }

    SqliteConnectionPool::getInstance().releaseConnection(db);
    return relationship;
}


std::optional<DbUserInfo> DatabaseManager::getUserInfoByUid(qint64 friend_uid) {
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    query.prepare(R"(
            SELECT * FROM user
            WHERE uid = :uid
        )");
    query.bindValue(":uid", friend_uid);

    DbUserInfo user;
    if (query.exec() && query.next()) {
        user.id = query.value("id").toInt();
        user.uid = query.value("uid").toLongLong();
        user.name = query.value("name").toString();
        user.email = query.value("email").toString();
        user.nick = query.value("nick").toString();
        user.desc = query.value("desc").toString();
        user.sex = query.value("sex").toInt();
        user.icon = query.value("icon").toString();
    } else {
        SqliteConnectionPool::getInstance().releaseConnection(db);
        return std::nullopt; // 如果查询不到数据，返回空值
    }

    SqliteConnectionPool::getInstance().releaseConnection(db);
    return user;
}

std::vector<DbUserInfo> DatabaseManager::getUserInfoByName(QString name) {
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    query.prepare(R"(
            SELECT * FROM user
            WHERE name LIKE :name
        )");
    query.bindValue(":name", "%"+name+"%");

    std::vector<DbUserInfo> users;

    if (query.exec()) {
        while (query.next()) {
            DbUserInfo user;
            user.id = query.value("id").toInt();
            user.uid = query.value("uid").toLongLong();
            user.name = query.value("name").toString();
            user.email = query.value("email").toString();
            user.nick = query.value("nick").toString();
            user.desc = query.value("desc").toString();
            user.sex = query.value("sex").toInt();
            user.icon = query.value("icon").toString();
            users.push_back(user);
        }
    }

    SqliteConnectionPool::getInstance().releaseConnection(db);
    return users;
}

std::optional<DbUserInfo> DatabaseManager::getUserInfoByEmail(QString email) {
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    query.prepare(R"(
            SELECT * FROM user
            WHERE email = :email
        )");
    query.bindValue(":email", email);

    DbUserInfo user;
    if (query.exec() && query.next()) {
        user.id = query.value("id").toInt();
        user.uid = query.value("uid").toLongLong();
        user.name = query.value("name").toString();
        user.email = query.value("email").toString();
        user.nick = query.value("nick").toString();
        user.desc = query.value("desc").toString();
        user.sex = query.value("sex").toInt();
        user.icon = query.value("icon").toString();
    } else {
        SqliteConnectionPool::getInstance().releaseConnection(db);
        return std::nullopt; // 如果查询不到数据，返回空值
    }

    SqliteConnectionPool::getInstance().releaseConnection(db);
    return user;
}

// 分页查询聊天记录
std::deque<ChatMessage> DatabaseManager::getChatMessages(qint64 user_id, qint64 friend_id, int limit, QString& timestamp) {
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    if (timestamp.isEmpty()) {
        query.prepare(R"(
                SELECT * FROM chat_messages
                WHERE (sender_id = :user_id AND receiver_id = :friend_id)
                   OR (sender_id = :friend_id AND receiver_id = :user_id)
                ORDER BY created_at DESC
                LIMIT :limit
            )");
    } else {
        query.prepare(R"(
                SELECT * FROM chat_messages
                WHERE ((sender_id = :user_id AND receiver_id = :friend_id)
                       OR (sender_id = :friend_id AND receiver_id = :user_id))
                  AND created_at < :timestamp
                ORDER BY created_at DESC
                LIMIT :limit
            )");
        query.bindValue(":timestamp", QDateTime::fromString(timestamp, Qt::ISODate).toString("yyyy-MM-dd HH:mm:ss"));
    }

    query.bindValue(":user_id", user_id);
    query.bindValue(":friend_id", friend_id);
    query.bindValue(":limit", limit);

    std::deque<ChatMessage> messages;
    QString lastTimestamp;

    if (query.exec()) {
        while (query.next()) {
            ChatMessage message = {};
            message.id = query.value("id").toInt();
            message.sender_id = query.value("sender_id").toLongLong();
            message.receiver_id = query.value("receiver_id").toLongLong();
            message.is_group = query.value("is_group").toBool();
            message.content = query.value("content").toString();
            message.content_type = query.value("content_type").toInt();
            message.status = query.value("status").toInt();
            message.created_at = query.value("created_at").toDateTime();
            message.updated_at = query.value("updated_at").toDateTime();
            messages.push_front(message);

            // 记录最后一条消息的时间戳
            lastTimestamp = message.created_at.toString(Qt::ISODate);
        }
    }

    // 在循环结束后更新 timestamp
    if (!lastTimestamp.isEmpty()) {
        timestamp = lastTimestamp;
    }

    SqliteConnectionPool::getInstance().releaseConnection(db);
    return messages;
}


std::optional<ChatMessage> DatabaseManager::getLastChatMessage(qint64 user_id, qint64 friend_id) {
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    query.prepare(R"(
            SELECT * FROM chat_messages
            WHERE ((sender_id = :user_id AND receiver_id = :friend_id)
                   OR (sender_id = :friend_id AND receiver_id = :user_id))
            ORDER BY created_at DESC
            LIMIT 1
        )");
    query.bindValue(":user_id", user_id);
    query.bindValue(":friend_id", friend_id);

    if (query.exec() && query.next()) {
        ChatMessage message;
        message.id = query.value("id").toInt();
        message.sender_id = query.value("sender_id").toLongLong();
        message.receiver_id = query.value("receiver_id").toLongLong();
        message.is_group = query.value("is_group").toBool();
        message.content = query.value("content").toString();
        message.content_type = query.value("content_type").toInt();
        message.status = query.value("status").toInt();
        message.created_at = query.value("created_at").toDateTime();
        message.updated_at = query.value("updated_at").toDateTime();

        SqliteConnectionPool::getInstance().releaseConnection(db);
        return message; // 返回封装的ChatMessage
    }

    SqliteConnectionPool::getInstance().releaseConnection(db);
    return std::nullopt; // 返回空的optional
}

int DatabaseManager::getUnreadMessageCountFromUser(qint64 uid, qint64 current_user_id, QVector<qint64>& unreadIds) {
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    // 查询未读信息的数量和ID集合
    query.prepare(R"(
            SELECT id
            FROM chat_messages
            WHERE sender_id = :uid AND receiver_id = :current_user_id AND status = 0
        )");
    query.bindValue(":uid", uid);
    query.bindValue(":current_user_id", current_user_id);

    int count = 0;
    unreadIds.clear(); // 清空引用参数，以确保没有残留数据

    if (query.exec()) {
        while (query.next()) {
            // 将未读消息的ID存入unreadIds
            unreadIds.append(query.value(0).toLongLong());
            count++;
        }
    } else {
        qWarning() << "Failed to get unread messages:" << query.lastError().text();
    }

    SqliteConnectionPool::getInstance().releaseConnection(db);
    return count;
}


void DatabaseManager::markMessagesAsRead(const std::vector<qint64>& messageIds) {
    if (messageIds.empty()) {
        qDebug() << "No message IDs provided to mark as read.";
        return;
    }

    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    // 构建动态 SQL 查询语句
    QStringList placeholders;
    for (size_t i = 0; i < messageIds.size(); ++i) {
        placeholders << "?";
    }
    QString sql = QString("UPDATE chat_messages SET status = 1 WHERE id IN (%1)").arg(placeholders.join(", "));

    query.prepare(sql);

    // 绑定参数
    for (size_t i = 0; i < messageIds.size(); ++i) {
        query.bindValue(i, messageIds[i]);
    }

    // 执行查询
    if (!query.exec()) {
        qDebug() << "Failed to mark messages as read:" << query.lastError().text();
        return;
    }

    qDebug() << "Successfully marked" << query.numRowsAffected() << "messages as read.";
}

void DatabaseManager::insertUser(qint64 uid, const QString& name, const QString& email, const QString& nick,
                                 const QString& desc, int sex, const QString& icon) {
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    query.prepare(R"(
            INSERT INTO user (uid, name, email, nick, `desc`, sex, icon)
            VALUES (:uid, :name, :email, :nick, :desc, :sex, :icon)
        )");
    query.bindValue(":uid", uid);
    query.bindValue(":name", name);
    query.bindValue(":email", email);
    query.bindValue(":nick", nick);
    query.bindValue(":desc", desc);
    query.bindValue(":sex", sex);
    query.bindValue(":icon", icon);

    if (!query.exec()) {
        qWarning() << "Insert user failed:" << query.lastError().text();
    }

    SqliteConnectionPool::getInstance().releaseConnection(db);
}

// 插入 friend_relationships 表数据
void DatabaseManager::insertFriendRelationship(qint64 user_id, qint64 friend_id, int status, const QString& created_at, const QString& updated_at) {
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    query.prepare(R"(
            INSERT INTO friend_relationships (user_id, friend_id, status, created_at, updated_at)
            VALUES (:user_id, :friend_id, :status, :created_at, :updated_at)
        )");
    query.bindValue(":user_id", user_id);
    query.bindValue(":friend_id", friend_id);
    query.bindValue(":status", status);
    query.bindValue(":created_at", created_at);
    query.bindValue(":updated_at", updated_at);

    if (!query.exec()) {
        qWarning() << "Insert friend relationship failed:" << query.lastError().text();
    }

    SqliteConnectionPool::getInstance().releaseConnection(db);
}

// 插入 friend_requests 表数据
void DatabaseManager::insertFriendRequest(qint64 id, qint64 sender_id, qint64 receiver_id, const QString& message, int status,
                                          const QString& created_at, const QString& updated_at, const QString& rejection_reason,
                                          bool is_sender_read) {
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    query.prepare(R"(
            INSERT INTO friend_requests (id, sender_id, receiver_id, message, status, created_at, updated_at, rejection_reason, is_sender_read)
            VALUES (:id, :sender_id, :receiver_id, :message, :status, :created_at, :updated_at, :rejection_reason, :is_sender_read)
        )");

    query.bindValue(":id", id);
    query.bindValue(":sender_id", sender_id);
    query.bindValue(":receiver_id", receiver_id);
    query.bindValue(":message", message);
    query.bindValue(":status", status);
    query.bindValue(":created_at", created_at);
    query.bindValue(":updated_at", updated_at);
    query.bindValue(":rejection_reason", rejection_reason);
    query.bindValue(":is_sender_read", is_sender_read);

    if (!query.exec()) {
        qWarning() << "Insert friend request failed:" << query.lastError().text();
    }

    SqliteConnectionPool::getInstance().releaseConnection(db);
}

// 插入 chat_messages 表数据
void DatabaseManager::insertChatMessage(qint64 id, qint64 sender_id, qint64 receiver_id, bool is_group, const QString& content, int content_type,
                                        int status, const QString& created_at, const QString& updated_at) {
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    query.prepare(R"(
            INSERT INTO chat_messages (id, sender_id, receiver_id, is_group, content, content_type, status, created_at, updated_at)
            VALUES (:id, :sender_id, :receiver_id, :is_group, :content, :content_type, :status, :created_at, :updated_at)
        )");
    query.bindValue(":id", id);
    query.bindValue(":sender_id", sender_id);
    query.bindValue(":receiver_id", receiver_id);
    query.bindValue(":is_group", is_group);
    query.bindValue(":content", content);
    query.bindValue(":content_type", content_type);
    query.bindValue(":status", status);
    query.bindValue(":created_at", created_at);
    query.bindValue(":updated_at", updated_at);

    if (!query.exec()) {
        qWarning() << "Insert chat message failed:" << query.lastError().text();
        SqliteConnectionPool::getInstance().releaseConnection(db);
    }
}



std::vector<FriendRequest> DatabaseManager::getMyFriendRequests(qint64 currentUserId) {
    std::vector<FriendRequest> requests;
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM friend_requests WHERE sender_id = ?");
    query.addBindValue(currentUserId);

    if (query.exec()) {
        while (query.next()) {
            FriendRequest request;
            request.id = query.value("id").toLongLong();
            request.sender_id = query.value("sender_id").toLongLong();
            request.receiver_id = query.value("receiver_id").toLongLong();
            request.message = query.value("message").toString();
            request.status = query.value("status").toInt();
            request.created_at = query.value("created_at").toString();
            request.updated_at = query.value("updated_at").toString();
            request.rejection_reason = query.value("rejection_reason").toString();
            request.is_sender_read = query.value("is_sender_read").toBool();
            requests.push_back(request);
        }
    } else {
        qDebug() << "Failed to query my friend requests:" << query.lastError().text();
    }
    return requests;
}

std::vector<FriendRequest> DatabaseManager::getNewFriendRequests(qint64 currentUserId) {
    std::vector<FriendRequest> requests;
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM friend_requests WHERE receiver_id = ? AND status = 0");
    query.addBindValue(currentUserId);

    if (query.exec()) {
        while (query.next()) {
            FriendRequest request;
            request.id = query.value("id").toLongLong();
            request.sender_id = query.value("sender_id").toLongLong();
            request.receiver_id = query.value("receiver_id").toLongLong();
            request.message = query.value("message").toString();
            request.status = query.value("status").toInt();
            request.created_at = query.value("created_at").toString();
            request.updated_at = query.value("updated_at").toString();
            request.rejection_reason = query.value("rejection_reason").toString();
            request.is_sender_read = query.value("is_sender_read").toBool();
            requests.push_back(request);
        }
    } else {
        qDebug() << "Failed to query new friend requests:" << query.lastError().text();
    }
    return requests;
}


QVector<qint64> DatabaseManager::getAllFriendIds() {
    // 从连接池获取数据库连接
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    // 准备 SQL 查询
    query.prepare(R"(
            SELECT friend_id
            FROM friend_relationships
        )");

    QVector<qint64> friendIds;

    // 执行查询并处理结果
    if (query.exec()) {
        while (query.next()) {
            friendIds.append(query.value(0).toLongLong());
        }
    } else {
        qWarning() << "Failed to get friend IDs:" << query.lastError().text();
    }

    // 释放数据库连接
    SqliteConnectionPool::getInstance().releaseConnection(db);

    return friendIds;
}

std::vector<DbUserInfo> DatabaseManager::getUserInfoByUids(const QVector<qint64>& uids) {
    // 从连接池获取数据库连接
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    // 动态生成占位符并绑定参数
    QStringList placeholders;
    for (size_t i = 0; i < uids.size(); ++i) {
        placeholders.append(":" + QString::number(i));
    }

    QString queryString = QString(R"(
            SELECT id, uid, name, nick, email, sex, icon, desc
            FROM user
            WHERE uid IN (%1)
        )").arg(placeholders.join(", "));

    query.prepare(queryString);

    for (size_t i = 0; i < uids.size(); ++i) {
        query.bindValue(":" + QString::number(i), uids[i]);
    }

    std::vector<DbUserInfo> userInfoList;

    // 执行查询并处理结果
    if (query.exec()) {
        while (query.next()) {
            DbUserInfo userInfo;
            userInfo.id = query.value("id").toInt();
            userInfo.uid = query.value("uid").toLongLong();
            userInfo.name = query.value("name").toString();
            userInfo.nick = query.value("nick").toString();
            userInfo.email = query.value("email").toString();
            userInfo.sex = query.value("sex").toInt();
            userInfo.icon = query.value("icon").toString();
            userInfo.desc = query.value("desc").toString();
            userInfoList.push_back(userInfo);
        }
    } else {
        qWarning() << "Failed to get user info:" << query.lastError().text();
    }

    // 释放数据库连接
    SqliteConnectionPool::getInstance().releaseConnection(db);

    return userInfoList;
}

QVector<qint64> DatabaseManager::getAllUnreadSenderIds(int currentUserId) {
    // 从连接池获取数据库连接
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    // 准备 SQL 查询
    query.prepare(R"(
            SELECT DISTINCT sender_id
            FROM chat_messages
            WHERE status = 0 AND receiver_id = ?
        )");
    query.addBindValue(currentUserId);

    QVector<qint64> senderIds;

    // 执行查询并处理结果
    if (query.exec()) {
        while (query.next()) {
            senderIds.append(query.value(0).toLongLong());
        }
    } else {
        qWarning() << "Failed to get unread sender IDs:" << query.lastError().text();
    }

    // 释放数据库连接
    SqliteConnectionPool::getInstance().releaseConnection(db);

    return senderIds;
}

std::optional<FriendRequest> DatabaseManager::getFriendRequestBySenderAndReceiver(qint64 senderId, qint64 receiverId) {
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    // 准备查询语句
    query.prepare("SELECT * FROM friend_requests WHERE sender_id = ? AND receiver_id = ?");
    query.addBindValue(senderId);
    query.addBindValue(receiverId);

    // 执行查询并处理结果
    if (query.exec()) {
        if (query.next()) {
            FriendRequest request;
            request.id = query.value("id").toLongLong();
            request.sender_id = query.value("sender_id").toLongLong();
            request.receiver_id = query.value("receiver_id").toLongLong();
            request.message = query.value("message").toString();
            request.status = query.value("status").toInt();
            request.created_at = query.value("created_at").toString();
            request.updated_at = query.value("updated_at").toString();
            request.rejection_reason = query.value("rejection_reason").toString();
            request.is_sender_read = query.value("is_sender_read").toBool();

            return request; // 返回查询到的好友请求信息
        }
    } else {
        qDebug() << "Failed to query friend request by sender and receiver:" << query.lastError().text();
    }

    return std::nullopt; // 如果没有查询到结果，返回空
}

bool DatabaseManager::setMessageStatusToRead(qint64 messageId) {
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    QSqlQuery query(db);

    // 准备 SQL 更新语句
    query.prepare(R"(
            UPDATE chat_messages
            SET status = 1
            WHERE id = :id
        )");
    query.bindValue(":id", messageId);

    // 执行查询并检查结果
    if (!query.exec()) {
        qWarning() << "Failed to update message status:" << query.lastError().text();
        SqliteConnectionPool::getInstance().releaseConnection(db);
        return false; // 更新失败
    }

    SqliteConnectionPool::getInstance().releaseConnection(db);
    return true; // 更新成功
}

std::vector<bool> DatabaseManager::doFriendsExist(const QVector<qint64> &uids) {
    // 如果集合为空，直接返回空的结果向量
    if (uids.isEmpty()) {
        return {};
    }

    // 从连接池获取数据库连接
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return std::vector<bool>(uids.size(), false);
    }

    // 准备结果向量
    std::vector<bool> results;
    results.reserve(uids.size());

    QSqlQuery query(db);

    // 遍历 QSet 的每个 uid 逐一查询
    for (auto uid : uids) {
        query.prepare(R"(
                SELECT 1 FROM friend_relationships
                WHERE friend_id = :uid
                LIMIT 1
            )");
        query.bindValue(":uid", uid);

        bool exists = false;
        if (query.exec() && query.next()) {
            exists = true; // 如果有结果，说明 uid 存在
        } else if (query.lastError().isValid()) {
            qWarning() << "SQL error for uid" << uid << ":" << query.lastError().text();
        }

        // 将结果加入向量
        results.push_back(exists);
    }

    // 释放数据库连接
    SqliteConnectionPool::getInstance().releaseConnection(db);
    return results;
}

bool DatabaseManager::doFriendsExist(const qint64 &uid)
{
    QSqlDatabase db = SqliteConnectionPool::getInstance().getConnection();
    if (!db.isOpen()) {
        qWarning() << "Database is not open!";
        return false;
    }

    QSqlQuery query(db);

    query.prepare(R"(
            SELECT 1 FROM friend_relationships
            WHERE friend_id = :uid
            LIMIT 1
        )");
    query.bindValue(":uid", uid);

    bool exists = false;
    if (query.exec() && query.next()) {
        exists = true; // 如果有结果，说明 uid 存在
    } else if (query.lastError().isValid()) {
        qWarning() << "SQL error for uid" << uid << ":" << query.lastError().text();
    }

    // 释放数据库连接
    SqliteConnectionPool::getInstance().releaseConnection(db);
    return exists;
}
