# cmake_minimum_required( VERSION 3.8.0 )

set(CMAKE_SYSTEM_NAME Linux )

SET (CMAKE_C_COMPILER               "gcc")
SET (CMAKE_C_FLAGS                  "-Wall")
SET (CMAKE_C_FLAGS_DEBUG            "-g")
SET (CMAKE_C_FLAGS_MINSIZEREL       "-Os -DNDEBUG")
SET (CMAKE_C_FLAGS_RELEASE          "-O3 -DNDEBUG")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g")

SET (CMAKE_CXX_COMPILER             "g++")
SET (CMAKE_CXX_FLAGS                "-Wall")
SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

#SET (CMAKE_AR      "ar")
#SET (CMAKE_LINKER  "ld")
#SET (CMAKE_NM      "nm")
#SET (CMAKE_OBJDUMP "objdump")
#SET (CMAKE_RANLIB  "ranlib")

