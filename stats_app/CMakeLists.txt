cmake_minimum_required(VERSION 3.10)
project(stats_app)

add_executable(stats_demo_app
    main.cpp
    Socket_receiver.cpp
    Stats_collector.cpp
)

target_link_libraries(stats_demo_app PRIVATE logger_static)

target_include_directories(stats_demo_app PRIVATE
    ${PROJECT_SOURCE_DIR}/Log_manager
    ${PROJECT_SOURCE_DIR}/stats_app)