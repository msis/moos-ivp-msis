
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(CODEBASE_JHU)

# Using absolute pathnames seems to work better then relative, because 
# you're never sure from which directory a relative path will be 
# employed in commands such as LINK_DIRECTORIES() ...



IF( ${WIN32} )
  # Windows does not have symbolic links. To handle this, subversion creates 
  # a text file that represents the symbolic link with the destination of the 
  # link inside the file. Here we extract the destination of the MOOS directory 
  # link and use it set a variable.
  FIND_FILE(MOOS_LINK MOOS ../../moos-ivp/trunk ../../moos-ivp/)
  FILE(READ ${MOOS_LINK} MOOS_LINK_DIR OFFSET 5)

  # For Windows, use the following paths
  GET_FILENAME_COMPONENT(MOOS_BASE_DIR_A ../../moos-ivp/trunk/${MOOS_LINK_DIR}
	                    ABSOLUTE)
  GET_FILENAME_COMPONENT(IVP_BASE_DIR_A  ../../moos-ivp/trunk/      ABSOLUTE)
  GET_FILENAME_COMPONENT(MOOS_BASE_DIR_B ../../moos-ivp/${MOOS_LINK_DIR}
	                    ABSOLUTE)
  GET_FILENAME_COMPONENT(IVP_BASE_DIR_B  ../../moos-ivp/            ABSOLUTE)

ELSE( ${WIN32} )
  # For Linux and Apple, use the following paths
  GET_FILENAME_COMPONENT(MOOS_BASE_DIR_A    ../../moos-ivp/trunk/MOOS  ABSOLUTE)
  GET_FILENAME_COMPONENT(IVP_BASE_DIR_A     ../../moos-ivp/trunk/      ABSOLUTE)
  GET_FILENAME_COMPONENT(MOOS_BASE_DIR_B    ../../moos-ivp/MOOS        ABSOLUTE)
  GET_FILENAME_COMPONENT(IVP_BASE_DIR_B     ../../moos-ivp/            ABSOLUTE)

ENDIF( ${WIN32} )

# CODEBASE_JHU_SOURCE_DIR is set by the PROJECT(...) command above.
SET( EXECUTABLE_OUTPUT_PATH ${CODEBASE_JHU_SOURCE_DIR}/../bin CACHE PATH
     "Output directory for the executables" )

#============================================================================
# Specify where to find MOOS's headers and libraries...
#============================================================================

SET(MOOS_LIB_DIR
   ${MOOS_BASE_DIR_A}/MOOSBin
   ${MOOS_BASE_DIR_B}/MOOSBin
   )

LINK_DIRECTORIES(${MOOS_LIB_DIR})

# No harm in giving all IvP software access to MOOS's include libraries.
# MOOS and IvP both have a copy of the 'newmat' library (different versions),
# but we don't seem to need to include MOOS's copy in our include path.
SET(MOOS_INCLUDE_DIRS
   ${MOOS_BASE_DIR_A}/Essentials
   ${MOOS_BASE_DIR_A}/Core
   ${MOOS_BASE_DIR_A}/Essentials/MOOSUtilityLib
   ${MOOS_BASE_DIR_A}/Core/MOOSLIB
   ${MOOS_BASE_DIR_A}/Core/MOOSGenLib

   ${MOOS_BASE_DIR_B}/Essentials
   ${MOOS_BASE_DIR_B}/Core
   ${MOOS_BASE_DIR_B}/Essentials/MOOSUtilityLib
   ${MOOS_BASE_DIR_B}/Core/MOOSLIB
   ${MOOS_BASE_DIR_B}/Core/MOOSGenLib
)

INCLUDE_DIRECTORIES(${MOOS_INCLUDE_DIRS})
   
#============================================================================
# Specify where to find IvP's headers and libraries...
#============================================================================

FILE(GLOB IVP_INCLUDE_DIRS 
   ${IVP_BASE_DIR_A}/ivp/src/lib_* 
   ${IVP_BASE_DIR_A}/ivp/src/lib_fltk-1.1.7/bin

   ${IVP_BASE_DIR_B}/ivp/src/lib_* 
   ${IVP_BASE_DIR_B}/ivp/src/lib_fltk-1.1.7/bin
   )

INCLUDE_DIRECTORIES(${IVP_INCLUDE_DIRS})

FILE(GLOB IVP_LIBRARY_DIRS 
   ${IVP_BASE_DIR_A}/lib
   ${IVP_BASE_DIR_A}/lib_fltk-1.1.7

   ${IVP_BASE_DIR_B}/lib 
   ${IVP_BASE_DIR_B}/lib_fltk-1.1.7
   )

LINK_DIRECTORIES(${IVP_LIBRARY_DIRS})

#============================================================================
# Specify where to find libraries local to this repository
#============================================================================

FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_*)

INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})
LINK_DIRECTORIES(${LOCAL_LIBRARY_DIRS})


#============================================================================
# Specify Compiler Flags
#============================================================================
IF( ${WIN32} )
  # Windows CXX_FLAGS

ELSE( ${WIN32} )
  # Linux and Apple CXX_Flags
  # Force -fPIC because gcc complains when we don't use it with x86_64 code.
  # Note sure why: -fPIC should only be needed for shared objects, and
  # AFAIK, CMake gets that right when building shared objects. -CJC
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g")
  IF( ${APPLE} )
    # FLTK is not supported in 64-bit for Mac OS X. Until it is supported
    # we will force all applications to build in 32-bit. Libtiff must also
    # be compiled in 32-bit. 
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
  ENDIF( ${APPLE} )
ENDIF( ${WIN32} )

IF(CMAKE_COMPILER_IS_GNUCXX)
    SET( WALL_ON OFF CACHE BOOL "tell me about all compiler warnings (-Wall) ")
    IF(WALL_ON)
	    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    ENDIF( WALL_ON)
ELSE(CMAKE_COMPILER_IS_GNUCXX)
    IF(MSVC)
	    SET( WALL_ON OFF CACHE BOOL "tell me about all compiler warnings (-Wall) ")
	    IF(WALL_ON)
	        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
		ENDIF(WALL_ON)
	ELSE(MSVC)
        # Other compilers go here	
	
	ENDIF(MSVC)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

#============================================================================
# List the subdirectories to build...
#============================================================================
ADD_SUBDIRECTORY(lib_behaviors-test)
ADD_SUBDIRECTORY(pXRelayTest)
