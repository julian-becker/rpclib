cmake_minimum_required(VERSION 3.5)
project(rpc_test LANGUAGES CXX)

find_package(rpc REQUIRED)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/test1.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_include_directories(${PROJECT_NAME}
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/catch2
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    rpc::rpc
)

add_test(
  NAME    ${PROJECT_NAME}
  COMMAND $<TARGET_FILE:${PROJECT_NAME}>
)
