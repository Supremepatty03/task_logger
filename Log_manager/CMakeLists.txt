# Источник
set(LOG_MANAGER_SRC Log_manager.cpp Log_writer.cpp Log_manager.hpp Log_writer.hpp)

# Статическая библиотека
add_library(logger_static STATIC ${LOG_MANAGER_SRC})

# Динамическая библиотека
add_library(logger_shared SHARED ${LOG_MANAGER_SRC})

# Общий include path
target_include_directories(logger_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(logger_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# одинаковое имя вывода
set_target_properties(logger_static PROPERTIES OUTPUT_NAME "logger")
set_target_properties(logger_shared PROPERTIES OUTPUT_NAME "logger")