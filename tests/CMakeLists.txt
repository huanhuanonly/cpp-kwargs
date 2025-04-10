#  ___    ___    ___       _   _  _       _  _____  ___    ___    ___   
# (  _`\ (  _`\ (  _`\    ( ) ( )( )  _  ( )(  _  )|  _`\ (  _`\ (  _`\ 
# | ( (_)| |_) )| |_) )   | |/'/'| | ( ) | || (_) || (_) )| ( (_)| (_(_)
# | |  _ | ,__/'| ,__/'   | , <  | | | | | ||  _  || ,  / | |___ `\__ \ 
# | (_( )| |    | |       | |\`\ | (_/ \_) || | | || |\ \ | (_, )( )_) |
# (____/'(_)    (_)       (_) (_)`\___x___/'(_) (_)(_) (_)(____/'`\____)
#                                                                       
#                         https://github.com/huanhuanonly/cpp-kwargs    
#                                                                       
# Copyright (c) 2024-2025 Yang Huanhuan (3347484963@qq.com).
# 
# Before using this file, please read its license:
#
#     https://github.com/huanhuanonly/cpp-kwargs/blob/main/LICENSE
#
# THIS  SOFTWARE  IS  PROVIDED  "AS IS",  WITHOUT  WARRANTY OF ANY KIND,
# EXPRESS  OR IMPLIED.  THE  AUTHOR  RESERVES  THE  RIGHT  TO MODIFY THE
# LICENSE OF THIS FILE AT ANY TIME.

message (STATUS "cpp-kwargs: ${CMAKE_CXX_COMPILER_ID}")

set (CMAKE_CXX_STANDARD 20)

if (CMAKE_CXX_STANDARD LESS 17)
    message (FATAL_ERROR "cpp-kwargs: c++17 or newer is required")
endif()

if (MSVC AND CMAKE_CXX_STANDARD LESS 20)
    add_compile_definitions (not=!)
    add_compile_definitions (and=&&)
    add_compile_definitions (or=||)
endif()

if (MSVC)
    add_compile_options(/Wall /permissive- /Zc:preprocessor)
endif()

add_executable (cpp-kwargs-test "test.cpp")

enable_testing ()

file (GLOB test_sources "test__*.cpp")

foreach (test_src ${test_sources})
    get_filename_component (test_name ${test_src} NAME_WE)
    add_executable (${test_name} ${test_src})
    add_test (NAME ${test_name} COMMAND ${test_name})
    set_tests_properties (${test_name} PROPERTIES TIMEOUT 3)
endforeach()

#  _                           _                                         _           #
# ( )                         ( )                                       (_ )         #
# | |__   _   _    _ _   ___  | |__   _   _    _ _   ___     _     ___   | |  _   _  #
# |  _ `\( ) ( ) /'_` )/' _ `\|  _ `\( ) ( ) /'_` )/' _ `\ /'_`\ /' _ `\ | | ( ) ( ) #
# | | | || (_) |( (_| || ( ) || | | || (_) |( (_| || ( ) |( (_) )| ( ) | | | | (_) | #
# (_) (_)`\___/'`\__,_)(_) (_)(_) (_)`\___/'`\__,_)(_) (_)`\___/'(_) (_)(___)`\__, | #
# https://github.com/huanhuanonly                                            ( )_| | #
#                                                                            `\___/' #