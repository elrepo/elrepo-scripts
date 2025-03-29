cmake_minimum_required(VERSION 3.26)

set (CMAKE_CXX_STANDARD 17)

project(nvidia-json)

find_package(jsoncpp REQUIRED)
get_target_property(jsoncpp_INCLUDE_DIRS jsoncpp_lib INTERFACE_INCLUDE_DIRECTORIES)
message("FOUND CONFIG: ${jsoncpp_CONFIG}")
message("INCLUDE DIRS: ${jsoncpp_INCLUDE_DIRS}")

add_executable(nvidia-json nvidia-json.cpp)
target_include_directories(nvidia-json PUBLIC ${jsoncpp_INCLUDE_DIRS})
target_link_libraries(nvidia-json PUBLIC jsoncpp_lib)
