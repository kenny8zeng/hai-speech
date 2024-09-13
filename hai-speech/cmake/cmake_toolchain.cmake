################################################################################
# CMAKE_MINIMUM_REQUIRED( VERSION 3.8 )

################################################################################
#
# 设置工具链
#

IF( NOT CMAKE_TOOLCHAIN_FILE )
    OPTIONS( TOOLCHAIN clang "Option toolchain" gcc clang arm aarch64 mips riscv )

    IF( ${TOOLCHAIN} STREQUAL "clang" )
        SET( CMAKE_TOOLCHAIN_FILE ${CMAKE_ROOT_LIST_DIR}/cmake/toolchain-clang.cmake )
    ELSEIF( ${TOOLCHAIN} STREQUAL "arm" )
        SET( CMAKE_TOOLCHAIN_FILE ${CMAKE_ROOT_LIST_DIR}/cmake/toolchain-arm.cmake )
    ELSEIF( ${TOOLCHAIN} STREQUAL "aarch64" )
        SET( CMAKE_TOOLCHAIN_FILE ${CMAKE_ROOT_LIST_DIR}/cmake/toolchain-aarch64.cmake )
    ELSEIF( ${TOOLCHAIN} STREQUAL "riscv" )
        SET( CMAKE_TOOLCHAIN_FILE ${CMAKE_ROOT_LIST_DIR}/cmake/toolchain-riscv.cmake )
    ELSEIF( ${TOOLCHAIN} STREQUAL "mips" )
        SET( CMAKE_TOOLCHAIN_FILE ${CMAKE_ROOT_LIST_DIR}/cmake/toolchain-mips.cmake )
    ELSE()
        SET( CMAKE_TOOLCHAIN_FILE ${CMAKE_ROOT_LIST_DIR}/cmake/toolchain-gcc.cmake )
    ENDIF()
ENDIF()

MESSAGE( STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}" )
