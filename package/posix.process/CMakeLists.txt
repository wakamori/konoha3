cmake_minimum_required(VERSION 2.6)
if(NOT MINGW AND NOT MSVC)
set(PACKAGE_SOURCE_CODE process_glue.c)
add_custom_command(OUTPUT
		${CMAKE_CURRENT_BINARY_DIR}/files/newfile.txt
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_CURRENT_SOURCE_DIR}/test/files/newfile.txt
		${CMAKE_CURRENT_BINARY_DIR}/files/newfile.txt)
add_custom_target(copy_process_test_files ALL DEPENDS
		${CMAKE_CURRENT_BINARY_DIR}/files/newfile.txt)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_konoha_package(posix.process)
endif()
