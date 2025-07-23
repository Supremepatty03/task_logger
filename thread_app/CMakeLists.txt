add_executable(thread_demo_app 
    main.cpp
    File_manager.cpp
    
)

target_link_libraries(thread_demo_app PRIVATE logger_static)
target_include_directories(thread_demo_app PRIVATE 
    ${PROJECT_SOURCE_DIR}/Log_manager
    ${PROJECT_SOURCE_DIR}/thread_app)