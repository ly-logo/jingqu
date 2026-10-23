#include "security/AuthService.h"

#include "core/Logger.h"
#include "core/TimeUtil.h"
#include "storage/Database.h"

#include <cstdio>
#include <cstdint>
#include <random>
#include <vector>

namespace scenicflow {

namespace {

constexpr int kTokenValidHours = 24;

std::string sha256FreeHash(const std::string& password) {
    // FNV-1a 64-bit with a fixed application salt.
    // Deliberately dependency-free: the project does not need a full crypto stack,
    // and this keeps the password out of the DB in plain text.
    uint64_t h = 1469598103934665603ULL;
    const std::string salted = "scenicflow::" + password;
    for (unsigned char c : salted) {
        h ^= static_cast<uint64_t>(c);
        h *= 1099511628211ULL;
    }
    char buf[24];
    std::snprintf(buf, sizeof(buf), "%016llx", static_cast<unsigned long long>(h));
    return std::string(buf);
}

// Usernames: 3-20 chars, letters/digits/underscore only (keeps them URL safe).
bool isValidUsername(const std::string& name) {
    if (name.size() < 3 || name.size() > 20) return false;
    for (char c : name) {
        const bool ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                        (c >= '0' && c <= '9') || c == '_';
        if (!ok) return false;
    }
    return true;
}

bool isValidPassword(const std::string& password) {
    return password.size() >= 6 && password.size() <= 32;
}

} // namespace

std::string AuthService::hashPassword(const std::string& password) {
    return sha256FreeHash(password);
}

AuthService& AuthService::instance() {
    static AuthService inst;
    return inst;
}

std::string AuthService::randomToken() {
    static thread_local std::mt19937_64 rng{std::random_device{}()};
    unsigned long long a = rng();
    unsigned long long b = rng();
    char buf[40];
    std::snprintf(buf, sizeof(buf), "%016llx%016llx", a, b);
    return std::string(buf);
}

void AuthService::seedUsers() {
    auto rows = Database::instance().query("SELECT COUNT(*) AS c FROM user");
    if (!rows.empty() && rows[0]["c"] != "0") return;

    // Demo accounts, password for all three is 123456.
    struct U { const char* name; const char* role; };
    const U users[] = {
        {"admin",     "admin"},
        {"operator",  "operator"},
        {"operator02","operator"},
        {"viewer",    "viewer"},
        {"guest01",   "viewer"},
    };
    const std::string hash = hashPassword("123456");
    for (const auto& u : users) {
        Database::instance().execParams(
            "INSERT INTO user(username,password_hash,role,created_at) VALUES(?,?,?,?)",
            {u.name, hash, u.role, nowString()});
    }
    logInfo("seeded user (5 rows, default password 123456)");
}

std::optional<LoginResult> AuthService::login(const std::string& username, const std::string& password) {
    auto rows = Database::instance().queryParams(
        "SELECT username, password_hash, role FROM user WHERE username=?", {username});
    if (rows.empty()) return std::nullopt;
    if (rows[0]["password_hash"] != hashPassword(password)) return std::nullopt;

    LoginResult result;
    result.username = rows[0]["username"];
    result.role = rows[0]["role"];
    result.token = randomToken();

    const std::string expire = formatTime(std::time(nullptr) + kTokenValidHours * 3600,
                                         "%Y-%m-%d %H:%M:%S");
    Database::instance().execParams(
        "INSERT INTO auth_token(token,username,role,expire_time) VALUES(?,?,?,?)",
        {result.token, result.username, result.role, expire});
    Database::instance().execParams("DELETE FROM auth_token WHERE expire_time < ?", {nowString()});
    logInfo("login ok: " + result.username + " (" + result.role + ")");
    return result;
}

RegisterResult AuthService::registerUser(const std::string& username, const std::string& password) {
    RegisterResult result;
    if (!isValidUsername(username)) {
        result.status = RegisterStatus::InvalidUsername;
        return result;
    }
    if (!isValidPassword(password)) {
        result.status = RegisterStatus::InvalidPassword;
        return result;
    }

    auto dup = Database::instance().queryParams("SELECT id FROM user WHERE username=?", {username});
    if (!dup.empty()) {
        result.status = RegisterStatus::Duplicate;
        return result;
    }

    // Self-service accounts are always viewers; admins/operators are granted
    // through the user management page.
    Database::instance().execParams(
        "INSERT INTO user(username,password_hash,role,created_at) VALUES(?,?,?,?)",
        {username, hashPassword(password), "viewer", nowString()});
    logInfo("user registered: " + username + " (viewer)");

    auto session = login(username, password);
    if (!session) {
        result.status = RegisterStatus::Duplicate;
        return result;
    }
    result.status = RegisterStatus::Ok;
    result.session = *session;
    return result;
}

std::optional<Session> AuthService::validate(const std::string& token) {
    if (token.empty()) return std::nullopt;
    auto rows = Database::instance().queryParams(
        "SELECT username, role, expire_time FROM auth_token WHERE token=?", {token});
    if (rows.empty()) return std::nullopt;
    if (rows[0]["expire_time"] < nowString()) {
        Database::instance().execParams("DELETE FROM auth_token WHERE token=?", {token});
        return std::nullopt;
    }
    return Session{rows[0]["username"], rows[0]["role"]};
}

void AuthService::logout(const std::string& token) {
    Database::instance().execParams("DELETE FROM auth_token WHERE token=?", {token});
}

bool AuthService::isPublicPath(const std::string& method, const std::string& path) {
    if (method == "OPTIONS") return true;
    if (path == "/api/auth/login" && method == "POST") return true;
    if (path == "/api/auth/register" && method == "POST") return true;
    if (path == "/api/health") return true;
    return false;
}

bool AuthService::canAccess(const std::string& role, const std::string& method,
                            const std::string& path) {
    // Logout is self-service: any logged-in role may drop its own token.
    if (method == "POST" && path == "/api/auth/logout") return true;
    if (role == "admin") return true;

    // operator: read-only across the whole admin surface.
    if (role == "operator") return method == "GET";

    // viewer: read-only, user-facing endpoints only.
    if (role == "viewer") {
        if (method != "GET") return false;
        static const char* kAllowed[] = {
            "/api/spots", "/api/parking", "/api/shuttles", "/api/system/status",
            "/api/events", "/api/statistics", "/api/scheduling", "/api/user",
        };
        for (const char* prefix : kAllowed) {
            // Match whole path segments only: "/api/user" must not leak "/api/users".
            const std::string base(prefix);
            if (path == base) return true;
            if (path.size() > base.size() && path.compare(0, base.size(), base) == 0 &&
                path[base.size()] == '/') {
                return true;
            }
        }
        return false;
    }
    return false;
}

} // namespace scenicflow
