################################################################################
CMAKE_MINIMUM_REQUIRED( VERSION 3.8 )

################################################################################
SET( CMAKE_ROOT_LIST_DIR ${CMAKE_CURRENT_LIST_DIR} )

################################################################################
INCLUDE( ${CMAKE_ROOT_LIST_DIR}/cmake/cmake_utility.cmake )
INCLUDE( ${CMAKE_ROOT_LIST_DIR}/cmake/cmake_toolchain.cmake )

################################################################################
# 导出编译命令描述
SET( CMAKE_EXPORT_COMPILE_COMMANDS 1 )

################################################################################
#
#  设置工程名及指示编译规则
#
#  cmake -DBUILD_DEBUG=ON ../src
#

PROJECT( hai-speech )

################################################################################
#
#  设置hude库路径
#
IF( NOT LIB_HUDE )
	SET( LIB_HUDE "${CMAKE_ROOT_LIST_DIR}/hude" )
ENDIF()

IF( NOT EXISTS "${LIB_HUDE}/test/test.hpp" )
	MESSAGE( FATAL_ERROR "can not find library: hude" )
ENDIF()

SET( INCLUDE_DIRS ${INCLUDE_DIRS}
	"${CMAKE_ROOT_LIST_DIR}/"
)

################################################################################
#
#  设置引用外部头件
#

SET( INCLUDE_DIRS ${INCLUDE_DIRS}
	"${CMAKE_ROOT_LIST_DIR}"
	"${CMAKE_ROOT_LIST_DIR}/tinyalsa/include"
    "${CMAKE_ROOT_LIST_DIR}/ifly/include"
	"${CMAKE_ROOT_LIST_DIR}/speech"
)

################################################################################
ADD_SUBDIRECTORY( ${CMAKE_ROOT_LIST_DIR}/tinyalsa )
ADD_SUBDIRECTORY( ${CMAKE_ROOT_LIST_DIR}/ifly )
ADD_SUBDIRECTORY( ${CMAKE_ROOT_LIST_DIR}/app )
