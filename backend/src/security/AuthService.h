#pragma once

#include <optional>
#include <string>

namespace scenicflow {

struct Session {
    std::string username;
    std::string role;   // admin / operator / viewer
};

struct LoginResult {
    std::string token;
    std::string username;
    std::string role;
};

// Result of a self-service sign-up. Registration always yields a viewer
// account: privileged roles can only be granted by an administrator.
enum class RegisterStatus {
    Ok,
    InvalidUsername,
    InvalidPassword,
    Duplicate,
};

struct RegisterResult {
    RegisterStatus status = RegisterStatus::Ok;
    LoginResult session;  // filled when status == Ok
};

// Token based auth backed by the auth_token table.
// Permission model (per project constraints): interface black/white list, no RBAC engine.
//   admin    -> all interfaces
//   operator -> read-only (GET only)
//   viewer   -> GET only on the user-facing endpoints
class AuthService {
public:
    static AuthService& instance();

    void seedUsers();

    std::optional<LoginResult> login(const std::string& username, const std::string& password);
    // Self-service sign-up. The created account is always a viewer.
    RegisterResult registerUser(const std::string& username, const std::string& password);
    std::optional<Session> validate(const std::string& token);
    void logout(const std::string& token);

    static bool isPublicPath(const std::string& method, const std::string& path);
    static bool canAccess(const std::string& role, const std::string& method, const std::string& path);

    static std::string hashPassword(const std::string& password);

private:
    static std::string randomToken();
};

} // namespace scenicflow
