#include "network/AuthController.h"

#include "network/ApiResult.h"
#include "security/AuthService.h"

namespace scenicflow {

namespace {

std::string bearerToken(const drogon::HttpRequestPtr& req) {
    const std::string header = req->getHeader("authorization");
    const std::string prefix = "Bearer ";
    if (header.size() > prefix.size() && header.compare(0, prefix.size(), prefix) == 0) {
        return header.substr(prefix.size());
    }
    return "";
}

} // namespace

void AuthController::login(const drogon::HttpRequestPtr& req,
                           std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto body = req->getJsonObject();
    if (!body || !(*body).isMember("username") || !(*body).isMember("password")) {
        callback(failed(400, "缺少用户名或密码"));
        return;
    }
    const std::string username = (*body)["username"].asString();
    const std::string password = (*body)["password"].asString();

    auto session = AuthService::instance().login(username, password);
    if (!session) {
        callback(failed(401, "用户名或密码错误"));
        return;
    }

    Json::Value data;
    data["token"] = session->token;
    data["role"] = session->role;
    data["username"] = session->username;
    callback(ok(data));
}

void AuthController::registerUser(const drogon::HttpRequestPtr& req,
                                  std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto body = req->getJsonObject();
    if (!body || !(*body).isMember("username") || !(*body).isMember("password")) {
        callback(failed(400, "缺少用户名或密码"));
        return;
    }
    const std::string username = (*body)["username"].asString();
    const std::string password = (*body)["password"].asString();

    const RegisterResult result = AuthService::instance().registerUser(username, password);
    switch (result.status) {
        case RegisterStatus::InvalidUsername:
            callback(failed(400, "用户名需为 3~20 位字母、数字或下划线"));
            return;
        case RegisterStatus::InvalidPassword:
            callback(failed(400, "密码长度需为 6~32 位"));
            return;
        case RegisterStatus::Duplicate:
            callback(failed(409, "用户名已存在"));
            return;
        case RegisterStatus::Ok:
            break;
    }

    Json::Value data;
    data["token"] = result.session.token;
    data["role"] = result.session.role;
    data["username"] = result.session.username;
    callback(ok(data));
}

void AuthController::logout(const drogon::HttpRequestPtr& req,
                            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    AuthService::instance().logout(bearerToken(req));
    callback(ok(Json::nullValue));
}

} // namespace scenicflow
