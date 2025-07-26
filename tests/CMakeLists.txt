include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(log_tests test_logmanager.cpp)

target_link_libraries(log_tests
  gtest_main
  logger_static
)
target_include_directories(log_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/Log_manager
)

include(GoogleTest)
gtest_discover_tests(log_tests)