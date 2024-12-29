cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("cpp-kwargs")

add_executable (cpp-kwargs "test.cpp" "CppKwargs.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET cpp-kwargs PROPERTY CXX_STANDARD 20)
endif()
