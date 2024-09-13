##########################################################################################
##    子项目模板    ########################################################################
##########################################################################################

################################################################################
#
#  设置项目名称
#

PROJECT(
    hai-speech-app
    LANGUAGES CXX C
    VERSION 0.1.1
)

################################################################################
#
#  设置构建类型
#       二进制： SET( BUILD_RULES EXECUTABLE )
#       静态库： SET( BUILD_RULES STATIC )
#       动态库： SET( BUILD_RULES SHARED )
#
#  OPTIONS( BUILD_RULES EXECUTABLE "Option build target of ${PROJECT_NAME}" EXECUTABLE STATIC SHARED )

SET( BUILD_RULES EXECUTABLE )

################################################################################
#
#  设置文件列表
#       GetFilesFromCurrentDir( ALL_FILE_LISTS )
#       GetFilesFromSpecifyDir( ALL_FILE_LISTS ${CMAKE_CURRENT_LIST_DIR} )

#       GetFilesFromCurrentDir( ALL_FILE_LISTS *.h *.hpp *.c *.cpp )
#       GetFilesFromSpecifyDir( ALL_FILE_LISTS ${CMAKE_CURRENT_LIST_DIR} *.h *.hpp *.c *.cpp )

#       GetFilesFromCurrentDir( ALL_FILE_LISTS main.hpp main.cpp )
#       GetFilesFromSpecifyDir( ALL_FILE_LISTS ${CMAKE_CURRENT_LIST_DIR} main.hpp main.cpp )

# GetFilesFromSpecifyDir( ALL_FILE_LISTS ${CMAKE_CURRENT_LIST_DIR}/linux *.h *.hpp *.c *.cpp )
GetFilesFromSpecifyDir( ALL_FILE_LISTS ${CMAKE_CURRENT_LIST_DIR} *.h *.hpp *.c *.cpp )

################################################################################
#
#  设置引用外部头件
#

# SET( INCLUDE_DIRS ${INCLUDE_DIRS} )

################################################################################
#
#  设置引用外部库
#

# SET( LINK_DIRS ${LINK_DIRS} ${CMAKE_ROOT_LIST_DIR}/ifly/libs/x64/ )

SET( LINK_LIBS ${LINK_LIBS} tinyalsa msc w_ivw )

################################################################################
#
#  设置自定义宏
#

# ADD_DEFINITIONS( xxx )

# SET( CMAKE_BASE_FLAGS 
# )
# 
# IF( CMAKE_COMPILER_IS_GNUCC )
#     SET( CMAKE_C_FLAGS   ${CMAKE_C_FLAGS}   ${CMAKE_BASE_FLAGS} )
# ENDIF( CMAKE_COMPILER_IS_GNUCC )
# 
# IF( CMAKE_COMPILER_IS_GNUCXX )
#     SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${CMAKE_BASE_FLAGS} )
# ENDIF( CMAKE_COMPILER_IS_GNUCXX )

################################################################################
#
#  开始构建
#

INCLUDE( ${CMAKE_ROOT_LIST_DIR}/cmake/cmake_build.cmake )
