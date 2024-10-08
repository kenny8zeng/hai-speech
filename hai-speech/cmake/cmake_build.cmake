# CMAKE_MINIMUM_REQUIRED( VERSION 3.8 )

################################################################################
#
#  设置是否生成调试信息
#
IF( NOT CMAKE_BUILD_TYPE )
	OPTION( BUILD_DEBUG "Build type DEBUG" OFF )
	IF( BUILD_DEBUG )
		SET( CMAKE_BUILD_TYPE Debug )
	ELSE()
		SET( CMAKE_BUILD_TYPE Release )
	ENDIF()
ENDIF()

MESSAGE( STATUS "Option build: ${CMAKE_BUILD_TYPE}" )

################################################################################
#
#  工程编译设置
#

OPTIONS( STANDARD_C   gnu11 "Option standard c"  c89 c99 c11 c17 gnu89 gnu99 gnu11 gnu17 )
SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=${STANDARD_C}" )

OPTIONS( STANDARD_CXX gnu++17 "Option standard c++"  c++89 c++99 c++11 c++17 c++20 c++23 gnu++89 gnu++99 gnu++11 gnu++17 gnu++20 gnu++23 )
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=${STANDARD_CXX}" )


IF( WIN32 )

	OPTION( WIN32_USE_CRT_STATIC_LIB "Using static C runtime library" ON )

	IF( WIN32_USE_CRT_STATIC_LIB )

		STRING( REGEX REPLACE "/MDd" "/MTd" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG} )

		STRING( REGEX REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE} )

		STRING( REGEX REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG} )

		STRING( REGEX REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE} )

	ENDIF( WIN32_USE_CRT_STATIC_LIB )

	ADD_DEFINITIONS( /W4 )

ELSEIF( UNIX AND CMAKE_COMPILER_IS_GNUCXX )

	IF( BUILD_RULES STREQUAL STATIC )

		ADD_DEFINITIONS( -fPIC )

	ELSEIF( BUILD_RULES STREQUAL SHARED )

		ADD_DEFINITIONS( -fPIC )

	ENDIF()

	OPTION( CXX_EMBEDDED_DEV "C++ Embedded development" OFF )

	IF( CXX_EMBEDDED_DEV )

		SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions )

	ENDIF( CXX_EMBEDDED_DEV )

	ADD_DEFINITIONS( -Wall )

ENDIF( WIN32 )

################################################################################
#
#  设置生成及安装规则
#

IF( NOT ALL_FILE_LISTS )

	MESSAGE( FATAL_ERROR "ERROR: SOURCE FILE IS EMPTY" )

ENDIF()

IF( ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC )

	SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
	SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
	SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

ELSE()

	SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
	SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
	SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

ENDIF()


IF( BASE_DEFINES )

	ADD_DEFINITIONS( ${BASE_DEFINES} )

ENDIF( BASE_DEFINES )

IF( INCLUDE_DIRS )

	INCLUDE_DIRECTORIES( ${INCLUDE_DIRS} )

ENDIF( INCLUDE_DIRS )

IF( LINK_DIRS )

	LINK_DIRECTORIES( ${LINK_DIRS} )

ENDIF( LINK_DIRS )

IF( BUILD_RULES STREQUAL EXECUTABLE )

	ADD_EXECUTABLE( ${PROJECT_NAME} ${ALL_FILE_LISTS} )

ELSEIF( BUILD_RULES STREQUAL STATIC )

	ADD_LIBRARY( ${PROJECT_NAME} STATIC ${ALL_FILE_LISTS} )

ELSEIF( BUILD_RULES STREQUAL SHARED )

	ADD_LIBRARY( ${PROJECT_NAME} SHARED ${ALL_FILE_LISTS} )

ELSE()

	MESSAGE( FATAL_ERROR "ERROR: NOT SET BUILD RULES" )

ENDIF()

IF( LINK_LIBS )

	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${LINK_LIBS} )

ENDIF( LINK_LIBS )

INSTALL( TARGETS ${PROJECT_NAME} DESTINATION bin )

