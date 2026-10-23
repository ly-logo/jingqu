#pragma once

#include <string>

namespace scenicflow {

// Creates tables from sql/schema.sql and inserts the initial seed data
// (matching the frontend mock) when the tables are empty.
class Schema {
public:
    static bool init(const std::string& schemaSqlPath);
};

} // namespace scenicflow
