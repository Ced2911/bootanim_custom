# custom boot animation for sega saturn bios

## how to compile tools

```
mkdir tools/build
cd tools/build
cmake .. -DCMAKE_INSTALL_PREFIX=../../.bin
make
```
## how to compile 

```
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../sh2.cmake
make
```

