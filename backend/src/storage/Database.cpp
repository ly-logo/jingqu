#include "storage/Database.h"

#include "core/Logger.h"

namespace scenicflow {

Database& Database::instance() {
    static Database inst;
    return inst;
}

bool Database::open(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    int rc = sqlite3_open_v2(path.c_str(), &m_db,
                             SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    if (rc != SQLITE_OK) {
        m_lastError = m_db ? sqlite3_errmsg(m_db) : "unknown error";
        if (m_db) {
            sqlite3_close(m_db);
            m_db = nullptr;
        }
        return false;
    }
    sqlite3_busy_timeout(m_db, 5000);
    sqlite3_exec(m_db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
    sqlite3_exec(m_db, "PRAGMA foreign_keys=ON;", nullptr, nullptr, nullptr);
    return true;
}

void Database::close() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool Database::exec(const std::string& sql) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_db) return false;
    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        m_lastError = errMsg ? errMsg : "exec failed";
        if (errMsg) sqlite3_free(errMsg);
        logError("sqlite exec failed: " + m_lastError);
        return false;
    }
    return true;
}

std::vector<Row> Database::query(const std::string& sql) {
    return queryParams(sql, {});
}

bool Database::execParams(const std::string& sql, const std::vector<std::string>& params) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_db) return false;
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        m_lastError = sqlite3_errmsg(m_db);
        logError("sqlite prepare failed: " + m_lastError);
        return false;
    }
    for (size_t i = 0; i < params.size(); ++i) {
        sqlite3_bind_text(stmt, static_cast<int>(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT);
    }
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        m_lastError = sqlite3_errmsg(m_db);
        logError("sqlite step failed: " + m_lastError);
        return false;
    }
    return true;
}

std::vector<Row> Database::queryParams(const std::string& sql, const std::vector<std::string>& params) {
    std::vector<Row> rows;
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_db) return rows;

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        m_lastError = sqlite3_errmsg(m_db);
        logError("sqlite prepare failed: " + m_lastError);
        return rows;
    }
    for (size_t i = 0; i < params.size(); ++i) {
        sqlite3_bind_text(stmt, static_cast<int>(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT);
    }

    int cols = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Row row;
        for (int c = 0; c < cols; ++c) {
            const char* name = sqlite3_column_name(stmt, c);
            const unsigned char* text = sqlite3_column_text(stmt, c);
            row[name ? name : ""] = text ? reinterpret_cast<const char*>(text) : "";
        }
        rows.push_back(std::move(row));
    }
    sqlite3_finalize(stmt);
    return rows;
}

long long Database::lastInsertId() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_db) return 0;
    return sqlite3_last_insert_rowid(m_db);
}

std::string Database::lastError() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_lastError;
}

} // namespace scenicflow
