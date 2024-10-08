# cmake_minimum_required( VERSION 3.8.0 )

set(CMAKE_SYSTEM_NAME Linux )

set(TOOLCHAIN_ARCH "arm-linux-gnueabihf-")

SET (CMAKE_C_COMPILER               "${TOOLCHAIN_ARCH}gcc")
SET (CMAKE_C_FLAGS                  "-Wall")
SET (CMAKE_C_FLAGS_DEBUG            "-g")
SET (CMAKE_C_FLAGS_MINSIZEREL       "-Os -DNDEBUG")
SET (CMAKE_C_FLAGS_RELEASE          "-O3 -DNDEBUG")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g")

SET (CMAKE_CXX_COMPILER             "${TOOLCHAIN_ARCH}g++")
SET (CMAKE_CXX_FLAGS                "-Wall")
SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

#SET (CMAKE_AR      "${TOOLCHAIN_ARCH}ar")
#SET (CMAKE_LINKER  "${TOOLCHAIN_ARCH}ld")
#SET (CMAKE_NM      "${TOOLCHAIN_ARCH}nm")
#SET (CMAKE_OBJDUMP "${TOOLCHAIN_ARCH}objdump")
#SET (CMAKE_RANLIB  "${TOOLCHAIN_ARCH}ranlib")

IF( TOOLCHAIN_SYSROOT )
    MESSAGE( STATUS "set <SYSROOT>: $ENV{TOOLCHAIN_SYS}" )
    SET (CMAKE_SYSROOT  ${TOOLCHAIN_SYSROOT})
ENDIF()

