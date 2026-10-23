#pragma once

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include <sqlite3.h>

namespace scenicflow {

using Row = std::unordered_map<std::string, std::string>;

// Thin thread-safe wrapper around the SQLite3 C API.
// A single connection is shared and every operation is serialized by a mutex.
// This is deliberately simple: the design does not need connection pooling at this scale.
class Database {
public:
    static Database& instance();

    bool open(const std::string& path);
    void close();

    bool exec(const std::string& sql);
    std::vector<Row> query(const std::string& sql);

    bool execParams(const std::string& sql, const std::vector<std::string>& params);
    std::vector<Row> queryParams(const std::string& sql, const std::vector<std::string>& params);

    long long lastInsertId();
    std::string lastError() const;

private:
    Database() = default;
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    sqlite3* m_db = nullptr;
    mutable std::mutex m_mutex;
    std::string m_lastError;
};

} // namespace scenicflow
