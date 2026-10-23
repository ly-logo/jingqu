cmake_minimum_required(VERSION 3.20)
project(ScenicFlowBackend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC: force UTF-8 source & execution charset (Chinese string literals / comments)
# NOMINMAX is required because drogon pulls in windows.h, whose min/max macros break std::min/max.
if(MSVC)
    add_compile_options(/utf-8 /W3 /MP)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS)
endif()

find_package(Drogon CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(Threads REQUIRED)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(scenicflow ${SRC_FILES})

target_include_directories(scenicflow PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(scenicflow PRIVATE
    Drogon::Drogon
    unofficial::sqlite3::sqlite3
    Threads::Threads
)

# Keep runtime resources (config/, sql/, data/) resolvable: run the exe from backend/.
