#pragma once

#include <drogon/HttpRequest.h>

#include <string>

namespace scenicflow {

// Drogon keeps matched path parameters in HttpRequest::getRoutingParameters()
// (ordered by the {placeholder} position in the route pattern), not in the
// query/form parameter map. Index is zero-based.
inline std::string pathParam(const drogon::HttpRequestPtr& req, size_t index = 0) {
    const auto& params = req->getRoutingParameters();
    if (index < params.size()) return params[index];
    return {};
}

} // namespace scenicflow
