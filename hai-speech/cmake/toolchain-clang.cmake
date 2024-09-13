# cmake_minimum_required( VERSION 3.8.0 )

set(CMAKE_SYSTEM_NAME Linux )

SET (CMAKE_C_COMPILER               "clang")
SET (CMAKE_C_FLAGS                  "-Wall")
SET (CMAKE_C_FLAGS_DEBUG            "-g")
SET (CMAKE_C_FLAGS_MINSIZEREL       "-Os -DNDEBUG")
SET (CMAKE_C_FLAGS_RELEASE          "-O3 -DNDEBUG")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g")

SET (CMAKE_CXX_COMPILER             "clang++")
SET (CMAKE_CXX_FLAGS                "-Wall")
SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

#SET (CMAKE_AR      "llvm-ar")
#SET (CMAKE_LINKER  "llvm-ld")
#SET (CMAKE_NM      "llvm-nm")
#SET (CMAKE_OBJDUMP "llvm-objdump")
#SET (CMAKE_RANLIB  "llvm-ranlib")

