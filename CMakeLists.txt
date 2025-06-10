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

option (KWARGS_KEY_CASE_INSENSITIVE "Enable case-insensitivity for KwargsKey" OFF)
option (KWARGS_USE_LITERAL_SUFFIX_NO_UNDERSCORE "Enable literal suffixes without leading underscores" OFF)
option (KWARGS_USE_SHORT_LITERAL_SUFFIX "Enable short literal suffix" OFF)
option (KWARGS_CONVERT_REALS_TO_STRINGS_WITH_HEURISTIC_PRECISION "Enable heuristic precision adjustment when converting floating point numbers to strings" OFF)

if (KWARGS_KEY_CASE_INSENSITIVE)
    add_compile_definitions (KWARGS_KEY_CASE_INSENSITIVE)
endif()

if (KWARGS_USE_LITERAL_SUFFIX_NO_UNDERSCORE)
    add_compile_definitions (KWARGS_USE_LITERAL_SUFFIX_NO_UNDERSCORE)
endif()

if (KWARGS_USE_SHORT_LITERAL_SUFFIX)
    add_compile_definitions (KWARGS_USE_SHORT_LITERAL_SUFFIX)
endif()

if (KWARGS_CONVERT_REALS_TO_STRINGS_WITH_HEURISTIC_PRECISION)
    add_compile_definitions (KWARGS_CONVERT_REALS_TO_STRINGS_WITH_HEURISTIC_PRECISION)
endif()

if (DEFINED KWARGS_CONVERT_REALS_TO_STRINGS_WITH_FIXED_PRECISION)
    add_compile_definitions (KWARGS_CONVERT_REALS_TO_STRINGS_WITH_FIXED_PRECISION=${KWARGS_CONVERT_REALS_TO_STRINGS_WITH_FIXED_PRECISION})
endif()

if (DEFINED KWARGS_CONVERT_REALS_TO_STRINGS_WITH_FIXED_FLOAT_PRECISION)
    add_compile_definitions (KWARGS_CONVERT_REALS_TO_STRINGS_WITH_FIXED_FLOAT_PRECISION=${KWARGS_CONVERT_REALS_TO_STRINGS_WITH_FIXED_FLOAT_PRECISION})
endif()

if (DEFINED KWARGS_CONVERT_REALS_TO_STRINGS_WITH_FIXED_DOUBLE_PRECISION)
    add_compile_definitions (KWARGS_CONVERT_REALS_TO_STRINGS_WITH_FIXED_DOUBLE_PRECISION=${KWARGS_CONVERT_REALS_TO_STRINGS_WITH_FIXED_DOUBLE_PRECISION})
endif()

if (DEFINED KWARGS_CONVERT_REALS_TO_STRINGS_WITH_FIXED_LONG_DOUBLE_PRECISION)
    add_compile_definitions (KWARGS_CONVERT_REALS_TO_STRINGS_WITH_FIXED_LONG_DOUBLE_PRECISION=${KWARGS_CONVERT_REALS_TO_STRINGS_WITH_FIXED_LONG_DOUBLE_PRECISION})
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