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

set (CMAKE_CXX_STANDARD 17)

message (STATUS "cpp-kwargs: ${CMAKE_CXX_COMPILER_ID}")

if (MSVC AND CMAKE_CXX_STANDARD LESS 20)
    add_compile_definitions (not=!)
    add_compile_definitions (and=&&)
    add_compile_definitions (or=||)
endif()

add_executable (cpp-kwargs-test "test.cpp")

#  _                           _                                         _           #
# ( )                         ( )                                       (_ )         #
# | |__   _   _    _ _   ___  | |__   _   _    _ _   ___     _     ___   | |  _   _  #
# |  _ `\( ) ( ) /'_` )/' _ `\|  _ `\( ) ( ) /'_` )/' _ `\ /'_`\ /' _ `\ | | ( ) ( ) #
# | | | || (_) |( (_| || ( ) || | | || (_) |( (_| || ( ) |( (_) )| ( ) | | | | (_) | #
# (_) (_)`\___/'`\__,_)(_) (_)(_) (_)`\___/'`\__,_)(_) (_)`\___/'(_) (_)(___)`\__, | #
# https://github.com/huanhuanonly                                            ( )_| | #
#                                                                            `\___/' #