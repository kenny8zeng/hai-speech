# CMAKE_MINIMUM_REQUIRED( VERSION 3.8 )

# GetFilesFromSpecifyDir

FUNCTION( GetFilesFromSpecifyDir result pathname )

	IF( NOT EXISTS ${pathname} )

		MESSAGE( WARNING "'${pathname}' NOT EXISTS" )

	ENDIF()

	SET( VALUES ${${result}} )

	IF( ${ARGC} EQUAL 2 )

		FOREACH( extname h hh hpp hxx c cc cpp cxx )

			FILE( GLOB_RECURSE RVALUE "${pathname}/*.${extname}" )

			IF( RVALUE )

				SET( VALUES ${VALUES} ${RVALUE} )

			ENDIF( RVALUE )

		ENDFOREACH( extname )

	ELSE()

		FOREACH( filename ${ARGN} )

			IF( ${filename} MATCHES "\\*" OR ${filename} MATCHES "\\?" )

				FILE( GLOB_RECURSE RVALUE "${pathname}/${filename}" )

				IF( RVALUE )

					SET( VALUES ${VALUES} ${RVALUE} )

				ENDIF( RVALUE )

			ELSE()

				IF( NOT EXISTS ${pathname}/${filename} )

					MESSAGE( WARNING "'${pathname}/${filename}' NOT EXISTS" )

				ENDIF()

				SET( VALUES ${VALUES} ${pathname}/${filename} )

			ENDIF()

		ENDFOREACH( filename )

	ENDIF()

	SET( ${result} ${VALUES} PARENT_SCOPE )

ENDFUNCTION( GetFilesFromSpecifyDir )

# GetFilesFromCurrentDir

FUNCTION( GetFilesFromCurrentDir result )

	SET( VALUES ${${result}} )

	GetFilesFromSpecifyDir( VALUES ${CMAKE_CURRENT_LIST_DIR} ${ARGN} )

	SET( ${result} ${VALUES} PARENT_SCOPE )

ENDFUNCTION( GetFilesFromCurrentDir )

# GetFilesFromSpecifyDirOfExclude

FUNCTION( GetFilesFromSpecifyDirOfExclude result pathname )

	SET( FILE_LISTS )

	GetFilesFromSpecifyDir( FILE_LISTS ${pathname} )

	SET( VALUES ${${result}} )

	FOREACH( filename ${FILE_LISTS} )

		SET( mark TRUE )

		FOREACH( item ${ARGN} )

			IF( ${filename} MATCHES ${item}$ )

				SET( mark FALSE )

				BREAK()

			ENDIF()

		ENDFOREACH( item )

		IF( mark )

			SET( VALUES ${VALUES} ${filename} )

		ENDIF()

	ENDFOREACH( filename )

	SET( ${result} ${VALUES} PARENT_SCOPE )

ENDFUNCTION( GetFilesFromSpecifyDirOfExclude )

# GetFilesFromCurrentDirOfExclude

FUNCTION( GetFilesFromCurrentDirOfExclude result )

	SET( VALUES ${${result}} )

	GetFilesFromSpecifyDirOfExclude( VALUES ${CMAKE_CURRENT_LIST_DIR} ${ARGN} )

	SET( ${result} ${VALUES} PARENT_SCOPE )

ENDFUNCTION( GetFilesFromCurrentDirOfExclude )

# OPTIONS

FUNCTION( OPTIONS result defval description )

	IF( ${result} )
		SET( defval ${${result}} )
	ENDIF()

	SET( _VAL_TMP "${result}_VAL_TMP" )

	SET( ${_VAL_TMP} ${defval} CACHE STRING ${description} )
	SET_PROPERTY(CACHE ${_VAL_TMP} PROPERTY STRINGS ${ARGN} )

	SET( ${result} ${${_VAL_TMP}} PARENT_SCOPE )

	MESSAGE( STATUS "${description}: ${${_VAL_TMP}}" )

ENDFUNCTION( OPTIONS )
