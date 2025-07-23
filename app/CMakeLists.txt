add_executable(demo_app main.cpp)

target_link_libraries(demo_app PRIVATE logger_static)
target_include_directories(demo_app PRIVATE ${PROJECT_SOURCE_DIR}/Log_manager)