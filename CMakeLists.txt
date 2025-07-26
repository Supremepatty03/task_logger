cmake_minimum_required(VERSION 3.10)
project(LoggerProject)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Log_manager)
add_subdirectory(app)
add_subdirectory(thread_app)
add_subdirectory(stats_app)
add_subdirectory(tests)

