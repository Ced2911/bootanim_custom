project(BootAnim)

enable_language(ASM)

set(BOOT_ANIM_SHARED
	${CMAKE_CURRENT_SOURCE_DIR}/crt0_bios.s
	${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

set(BOOT_ANIM_CARD
	${CMAKE_CURRENT_SOURCE_DIR}/crt0_card.s
)

add_definitions(-DFENRIR_VERSION=\"${FENRIR_VERSION}\")
add_definitions(-DREENTRANT_SYSCALLS_PROVIDED)
add_definitions(-DMISSING_SYSCALL_NAMES)

include("../Cmake.in")
add_saturn_executable(BootAnim "${BOOT_ANIM_SHARED}" ${CMAKE_CURRENT_SOURCE_DIR}/comp.ld)
add_dependencies(BootAnim iapetus)

add_custom_command(
	TARGET BootAnim POST_BUILD
	COMMAND ${CMAKE_SOURCE_DIR}/.bin/bin/fastlz -1 ${CMAKE_BINARY_DIR}/BootAnim.bin ${CMAKE_BINARY_DIR}/bootanim.fz
)

add_custom_command(
	TARGET BootAnim POST_BUILD
	COMMAND ${CMAKE_SOURCE_DIR}/.bin/bin/quickcmp ${CMAKE_BINARY_DIR}/BootAnim.bin ${CMAKE_BINARY_DIR}/bootanim.qz
)

add_custom_command(
	TARGET BootAnim POST_BUILD
	COMMAND xxd -i ${CMAKE_BINARY_DIR}/bootanim.fz ${CMAKE_BINARY_DIR}/bootanim.h  
)
#add_custom_command(
#	TARGET BootAnim POST_BUILD
#	COMMAND xxd -i ${CMAKE_BINARY_DIR}/bootanim.qz ${CMAKE_BINARY_DIR}/bootanim.h
#)
