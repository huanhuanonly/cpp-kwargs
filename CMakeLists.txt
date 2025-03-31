cmake_minimum_required (VERSION 3.8)

include_directories ("include/")

if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
	message (STATUS "cpp-kwargs: [testing...].")
	add_subdirectory ("test/")
else()
	message (STATUS "cpp-kwargs: [included].")
endif()