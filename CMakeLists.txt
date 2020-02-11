cmake_minimum_required(VERSION 3.5)
project(rpcdev)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_LIST_DIR}/cmake)
list(INSERT CMAKE_PREFIX_PATH 0 ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(AddModule)


add_module(NAME rpc)

enable_testing()
add_module(NAME rpc_test)
