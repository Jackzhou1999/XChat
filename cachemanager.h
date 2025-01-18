#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <QString>
#include <QDateTime>
#include <unordered_map>
#include <list>
#include <mutex>
#include "userdata.h"


// LRU缓存模板
template<typename Key, typename Value>
class LRUCache {
private:
    size_t max_size;
    std::list<std::pair<Key, Value>> cache_list;
    std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> cache_map;
    std::mutex mutex;

public:
    explicit LRUCache(size_t max_size) : max_size(max_size) {}

    std::optional<Value> get(const Key& key) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = cache_map.find(key);
        if (it == cache_map.end()) {
            return std::nullopt;
        }
        // Move accessed item to the front
        cache_list.splice(cache_list.begin(), cache_list, it->second);
        return it->second->second;
    }

    void put(const Key& key, const Value& value) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = cache_map.find(key);
        if (it != cache_map.end()) {
            // Update existing item
            cache_list.erase(it->second);
            cache_map.erase(it);
        }
        cache_list.emplace_front(key, value);
        cache_map[key] = cache_list.begin();

        // Remove least recently used item if over capacity
        if (cache_list.size() > max_size) {
            auto last = cache_list.end();
            --last;
            cache_map.erase(last->first);
            cache_list.pop_back();
        }
    }
};

// 缓存单例类
class CacheManager {
private:
    // 最大缓存大小
    const size_t user_cache_size = 100; // 可根据需求调整
    const size_t chat_message_queue_max_size = 100;

    // LRU 缓存用于 UserInfo
    LRUCache<qint64, DbUserInfo> user_cache;

    // 聊天消息缓存，Key为对方UID，Value为消息队列
    std::unordered_map<qint64, std::list<ChatMessage>> chat_message_cache;
    std::mutex chat_mutex;

    CacheManager() : user_cache(user_cache_size) {}

public:
    // 获取单例实例
    static CacheManager& getInstance() {
        static CacheManager instance;
        return instance;
    }

    // 禁用拷贝和赋值
    CacheManager(const CacheManager&) = delete;
    CacheManager& operator=(const CacheManager&) = delete;

    // 用户信息操作
    std::optional<DbUserInfo> getUserInfo(qint64 uid) {
        return user_cache.get(uid);
    }

    void putUserInfo(const DbUserInfo& user) {
        user_cache.put(user.uid, user);
    }

    // 聊天消息操作
    void addChatMessage(qint64 uid, const ChatMessage& message) {
        std::lock_guard<std::mutex> lock(chat_mutex);
        auto& message_queue = chat_message_cache[uid];
        message_queue.push_front(message);

        // 如果队列超过最大大小，删除最旧的消息
        if (message_queue.size() > chat_message_queue_max_size) {
            message_queue.pop_back();
        }
    }

    std::list<ChatMessage> getChatMessages(qint64 uid) {
        std::lock_guard<std::mutex> lock(chat_mutex);
        if (chat_message_cache.find(uid) != chat_message_cache.end()) {
            return chat_message_cache[uid];
        }
        return {};
    }
};
#endif // CACHEMANAGER_H
