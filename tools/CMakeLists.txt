project(tools)

include_directories(../bl/fastlz/ ../bl/quicklz/)

set(6PACK_SRC
	../bl/fastlz/fastlz.c
	fastlz.main.c
)
add_executable(fastlz "${6PACK_SRC}")
install(TARGETS fastlz RUNTIME)

set(QUICKLZ
	../bl/quicklz/quicklz.c	
	compress_file.c
)
add_executable(quickcmp "${QUICKLZ}")
install(TARGETS quickcmp RUNTIME)
