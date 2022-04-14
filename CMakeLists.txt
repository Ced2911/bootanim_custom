CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

project(BootAnimCustom)

include(ExternalProject)
		
set(IAPETUS_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/iapetus)
set(IAPETUS_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/iapetus/build/)
ExternalProject_Add(iapetus
      SOURCE_DIR "${IAPETUS_SOURCE_DIR}"
      BINARY_DIR "${IAPETUS_BINARY_DIR}"
      CMAKE_ARGS "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}"
                  "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}"
                  "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
                  "-DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}"
      INSTALL_COMMAND "" 
)

ExternalProject_Get_Property(iapetus source_dir)
set(IAPETUS_INCLUDE_DIR ${source_dir}/src)

include_directories(${IAPETUS_INCLUDE_DIR})

set(IAPETUS_LIB "${IAPETUS_BINARY_DIR}/src/libiapetus.a")

find_program(CMAKE_MKISOFS NAMES mkisofs)

find_package(Git)
execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY "${local_dir}"
    OUTPUT_VARIABLE _build_version
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

string(TIMESTAMP _time_stamp)

message( STATUS "GIT hash: ${_build_version}")

configure_file(src/version.h.in version.h @ONLY)

add_subdirectory(src)
add_subdirectory(bl)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
