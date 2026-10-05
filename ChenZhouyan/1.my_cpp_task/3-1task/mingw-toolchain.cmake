# 强制使用 mingw64 编译器，覆盖任何 kit 的默认选择（含 clang）
set(CMAKE_C_COMPILER   "C:/mingw64/bin/gcc.exe"   CACHE FILEPATH "C compiler" FORCE)
set(CMAKE_CXX_COMPILER "C:/mingw64/bin/g++.exe"   CACHE FILEPATH "CXX compiler" FORCE)
