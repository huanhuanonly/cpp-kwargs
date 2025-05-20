#  ___    ___    ___       _   _  _       _  _____  ___    ___    ___   
# (  _`\ (  _`\ (  _`\    ( ) ( )( )  _  ( )(  _  )|  _`\ (  _`\ (  _`\.
# | ( (_)| |_) )| |_) )   | |/'/'| | ( ) | || (_) || (_) )| ( (_)| (_(_)
# | |  _ | ,__/'| ,__/'   | , <  | | | | | ||  _  || ,  / | |___ `\__ \.
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

cmake_minimum_required (VERSION 3.8)

include_directories ("include/")

if (DEFINED KWARGSKEY_CASE_INSENSITIVE)
    add_compile_definitions (KWARGSKEY_CASE_INSENSITIVE)
endif()

if (DEFINED KWARGSKEY_LITERAL_SUFFIX_WITHOUT_LEADING_UNDERSCORE)
    add_compile_definitions (KWARGSKEY_LITERAL_SUFFIX_WITHOUT_LEADING_UNDERSCORE)
endif()

if (DEFINED KWARGSKEY_LITERAL_SHORT_SUFFIX)
    add_compile_definitions (KWARGSKEY_LITERAL_SHORT_SUFFIX)
endif()


if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    message (STATUS "cpp-kwargs: [testing...].")

    include (CTest)
    enable_testing ()

    add_subdirectory ("tests/")
else()
    message (STATUS "cpp-kwargs: [included].")
endif()

#  _                           _                                         _           #
# ( )                         ( )                                       (_ )         #
# | |__   _   _    _ _   ___  | |__   _   _    _ _   ___     _     ___   | |  _   _  #
# |  _ `\( ) ( ) /'_` )/' _ `\|  _ `\( ) ( ) /'_` )/' _ `\ /'_`\ /' _ `\ | | ( ) ( ) #
# | | | || (_) |( (_| || ( ) || | | || (_) |( (_| || ( ) |( (_) )| ( ) | | | | (_) | #
# (_) (_)`\___/'`\__,_)(_) (_)(_) (_)`\___/'`\__,_)(_) (_)`\___/'(_) (_)(___)`\__, | #
# https://github.com/huanhuanonly                                            ( )_| | #
#                                                                            `\___/' #