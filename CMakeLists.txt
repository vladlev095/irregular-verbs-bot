cmake_minimum_required(VERSION 3.5)
project(MyTelegramBot)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

find_package(TgBot REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(nlohmann_json REQUIRED)

file(GLOB SRC_FILES src/*.cpp)
add_executable(MyTelegramBot ${SRC_FILES})

target_link_libraries(MyTelegramBot TgBot::TgBot nlohmann_json::nlohmann_json)