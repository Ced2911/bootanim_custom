project(bootloader)

enable_language(ASM)

set(LOADER_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/crt0_bl.s
	${CMAKE_CURRENT_SOURCE_DIR}/main.c
	${CMAKE_CURRENT_SOURCE_DIR}/fastlz/fastlz.c
#	${CMAKE_CURRENT_SOURCE_DIR}/quicklz/quicklz.c
)

add_definitions(-DFENRIR_VERSION=\"${FENRIR_VERSION}\")
add_definitions(-DREENTRANT_SYSCALLS_PROVIDED)
add_definitions(-DMISSING_SYSCALL_NAMES)

include("../Cmake.in")
add_saturn_executable(bootloader "${LOADER_SRC}" ${CMAKE_CURRENT_SOURCE_DIR}/loader.ld)
# add_executable(bootloader ${LOADER_SRC})
# target_link_options(bootloader PRIVATE -T${CMAKE_CURRENT_SOURCE_DIR}/loader.ld)
# target_include_directories(bootloader PRIVATE ${CMAKE_BINARY_DIR})
