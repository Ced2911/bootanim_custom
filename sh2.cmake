#include(CMakeForceCompiler)

# Look for GCC in path
FIND_FILE( SH_GCC_COMPILER_EXE "sh-elf-gcc.exe" PATHS ENV INCLUDE)
FIND_FILE( SH_GCC_COMPILER "sh-elf-gcc" PATHS ENV INCLUDE)

# Select which is found
if (EXISTS ${SH_GCC_COMPILER})
set( SH2_GCC_COMPILER ${SH_GCC_COMPILER})
elseif (EXISTS ${SH_GCC_COMPILER_EXE})
set( SH2_GCC_COMPILER ${SH_GCC_COMPILER_EXE})
else()
message(FATAL_ERROR "SH2 GCC not found. ${SH_GCC_COMPILER_EXE} ${SH_GCC_COMPILER}")
endif()

message( "SH2 GCC found: ${SH2_GCC_COMPILER}")

get_filename_component(SH2_TOOLCHAIN_BIN_PATH ${SH2_GCC_COMPILER} DIRECTORY)
get_filename_component(SH2_TOOLCHAIN_BIN_GCC ${SH2_GCC_COMPILER} NAME_WE)
get_filename_component(SH2_TOOLCHAIN_BIN_EXT ${SH2_GCC_COMPILER} EXT)

message( "SH2 GCC Path: ${SH2_TOOLCHAIN_BIN_PATH}" )

STRING(REGEX REPLACE "\-gcc" "-" CROSS_COMPILE ${SH2_TOOLCHAIN_BIN_GCC})
message( "SH2 Cross Compile: ${CROSS_COMPILE}" )

# The Generic system name is used for embedded targets (targets without OS) in
# CMake
set( CMAKE_SYSTEM_NAME          Generic )
set( CMAKE_SYSTEM_PROCESSOR     sh2 )
set( CMAKE_EXECUTABLE_SUFFIX    ".elf")

# specify the cross compiler. We force the compiler so that CMake doesn't
# attempt to build a simple test program as this will fail without us using
# the -nostartfiles option on the command line
#CMAKE_FORCE_C_COMPILER( "${SH2_TOOLCHAIN_BIN_PATH}/${CROSS_COMPILE}gcc${SH2_TOOLCHAIN_BIN_EXT}" GNU )
#CMAKE_FORCE_CXX_COMPILER( "${SH2_TOOLCHAIN_BIN_PATH}/${CROSS_COMPILE}g++${SH2_TOOLCHAIN_BIN_EXT}" GNU )
set(CMAKE_ASM_COMPILER {CROSS_COMPILE}gcc )
set(CMAKE_AR ${CROSS_COMPILE}ar)
set(CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc)
set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)

# We must set the OBJCOPY setting into cache so that it's available to the
# whole project. Otherwise, this does not get set into the CACHE and therefore
# the build doesn't know what the OBJCOPY filepath is
set( CMAKE_OBJCOPY      ${SH2_TOOLCHAIN_BIN_PATH}/${CROSS_COMPILE}objcopy
     CACHE FILEPATH "The toolchain objcopy command " FORCE )

set( CMAKE_OBJDUMP      ${SH2_TOOLCHAIN_BIN_PATH}/${CROSS_COMPILE}objdump
     CACHE FILEPATH "The toolchain objdump command " FORCE )

# Set the common build flags

# Set the CMAKE C flags (which should also be used by the assembler!
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m2" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections" )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" )
set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" )
set( CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" )
set( CMAKE_EXE_LINKER_FLAGS   "${CMAKE_EXE_LINKER_FLAGS}  -m2 -nostartfiles -Wl,--gc-sections" )
