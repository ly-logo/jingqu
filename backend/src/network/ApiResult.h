#pragma once

#include <json/json.h>

#include <drogon/HttpResponse.h>

namespace scenicflow {

// Unified response envelope: { code, message, data }
// CORS headers are added globally by the post-handling advice in main.cpp.
inline drogon::HttpResponsePtr jsonResult(int code, const std::string& message, const Json::Value& data) {
    Json::Value root;
    root["code"] = code;
    root["message"] = message;
    root["data"] = data;
    auto resp = drogon::HttpResponse::newHttpJsonResponse(root);
    resp->setStatusCode(static_cast<drogon::HttpStatusCode>(code));
    return resp;
}

inline drogon::HttpResponsePtr ok(const Json::Value& data = Json::nullValue) {
    return jsonResult(200, "success", data);
}

inline drogon::HttpResponsePtr failed(int code, const std::string& message) {
    return jsonResult(code, message, Json::nullValue);
}

} // namespace scenicflow
