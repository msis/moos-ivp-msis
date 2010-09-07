##############################################################################
# FILE:        moos-ivp-extend/CMakeLists.txt
# DATE:        2010/09/07
# DESCRIPTION: Top-level CMakeLists.txt file for the moos-ivp-extend project
##############################################################################


# We are still supporting version 2.6. However, we will be upgrade to 
# CMake 2.8 soon. Upgrade to version 2.8 if you can.
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT( IVP_EXTEND )

#=============================================================================
# Set the output directories for the binary and library files
#=============================================================================

GET_FILENAME_COMPONENT(IVP_EXTEND_BIN_DIR "${CMAKE_SOURCE_DIR}/bin"
   ABSOLUTE )
GET_FILENAME_COMPONENT(IVP_EXTEND_LIB_DIR "${CMAKE_SOURCE_DIR}/lib"
   ABSOLUTE )

SET( LIBRARY_OUTPUT_PATH "${IVP_EXTEND_LIB_DIR}" CACHE PATH "" )
SET( ARCHIVE_OUTPUT_DIRECTORY "${IVP_EXTEND_LIB_DIR}" CACHE PATH "" )
SET( LIBRARY_OUTPUT_DIRECTORY "${IVP_EXTEND_LIB_DIR}" CACHE PATH "" )

SET( EXECUTABLE_OUTPUT_PATH "${IVP_EXTEND_BIN_DIR}" CACHE PATH "" )
SET( RUNTIME_OUTPUT_DIRECTORY "${IVP_EXTEND_BIN_DIR}" CACHE PATH "" )

#=============================================================================
# Find the MOOS and IvP Directories/Header files
#=============================================================================
# Using absolute pathnames seems to work better then relative, because 
# you're never sure from which directory a relative path will be 
# employed in commands such as LINK_DIRECTORIES() ...

# At the moment, we expect the moos-ivp repository to be up one directory
SET(MOOSIVP_DIR "../moos-ivp" CACHE PATH "Path to the MOOS-IvP directory")

IF( ${WIN32} )
  # Windows does not have symbolic links. To handle this, subversion creates 
  # a text file that represents the symbolic link with the destination of the 
  # link inside the file. Here we extract the destination of the MOOS directory 
  # link and use it set a variable.
  FIND_FILE(MOOS_LINK MOOS ${MOOSIVP_DIR}/trunk ${MOOSIVP_DIR}/)
  FILE(READ ${MOOS_LINK} MOOS_LINK_DIR OFFSET 5)

  # For Windows, use the following paths
   GET_FILENAME_COMPONENT(MOOS_BASE_DIR_A 
      "${MOOSIVP_DIR}/trunk/${MOOS_LINK_DIR}" ABSOLUTE)
   GET_FILENAME_COMPONENT(IVP_BASE_DIR_A "${MOOSIVP_DIR}/trunk/" ABSOLUTE)
   GET_FILENAME_COMPONENT(MOOS_BASE_DIR_B "${MOOSIVP_DIR}/${MOOS_LINK_DIR}"
      ABSOLUTE)
   GET_FILENAME_COMPONENT(IVP_BASE_DIR_B "${MOOSIVP_DIR}/" ABSOLUTE)

ELSE( ${WIN32} )
   # For Linux and Apple, use the following paths
   GET_FILENAME_COMPONENT(MOOS_BASE_DIR_A "${MOOSIVP_DIR}/trunk/MOOS" ABSOLUTE)
   GET_FILENAME_COMPONENT(IVP_BASE_DIR_A "${MOOSIVP_DIR}/trunk/" ABSOLUTE)
   GET_FILENAME_COMPONENT(MOOS_BASE_DIR_B "${MOOSIVP_DIR}/MOOS" ABSOLUTE)
   GET_FILENAME_COMPONENT(IVP_BASE_DIR_B "${MOOSIVP_DIR}/" ABSOLUTE)

ENDIF( ${WIN32} )

#-----------------------------------------------------------------------------
# Specify where to find MOOS's headers and libraries...
#-----------------------------------------------------------------------------

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
   
#-----------------------------------------------------------------------------
# Specify where to find IvP's headers and libraries...
#-----------------------------------------------------------------------------

FILE(GLOB IVP_INCLUDE_DIRS 
   ${IVP_BASE_DIR_A}/ivp/src/lib_* 
   ${IVP_BASE_DIR_A}/ivp/src/lib_fltk-1.1.10/bin

   ${IVP_BASE_DIR_B}/ivp/src/lib_* 
   ${IVP_BASE_DIR_B}/ivp/src/lib_fltk-1.1.10/bin
   )

INCLUDE_DIRECTORIES(${IVP_INCLUDE_DIRS})

FILE(GLOB IVP_LIBRARY_DIRS 
   ${IVP_BASE_DIR_A}/lib
   ${IVP_BASE_DIR_A}/lib_fltk-1.1.10

   ${IVP_BASE_DIR_B}/lib 
   ${IVP_BASE_DIR_B}/lib_fltk-1.1.10
   )

LINK_DIRECTORIES(${IVP_LIBRARY_DIRS})

#=============================================================================
# Specify Compiler Flags
#=============================================================================
IF( ${WIN32} )
   #---------------------------------------------
   # Windows Compiler Flags
   #---------------------------------------------
   IF(MSVC)
      # Flags for Microsoft Visual Studio
      SET( WALL_ON OFF CACHE BOOL 
         "tell me about all compiler warnings (-Wall) ")
      IF(WALL_ON)
         SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
      ENDIF(WALL_ON)
   ELSE(MSVC)
      # Other Windows compilers go here
   ENDIF(MSVC)

ELSE( ${WIN32} )
   #---------------------------------------------
   # Linux and Apple Compiler Flags
   #---------------------------------------------
   # Force -fPIC because gcc complains when we don't use it with x86_64 code.
   # Note sure why: -fPIC should only be needed for shared objects, and
   # AFAIK, CMake gets that right when building shared objects. -CJC
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g")
   IF(CMAKE_COMPILER_IS_GNUCXX)
      # Flags for the GNU C++ Compiler
      SET( WALL_ON OFF CACHE BOOL 
         "tell me about all compiler warnings (-Wall) ")
      IF(WALL_ON)
         SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
      ENDIF( WALL_ON)
   ELSE(CMAKE_COMPILER_IS_GNUCXX)
    
   ENDIF(CMAKE_COMPILER_IS_GNUCXX)

  IF( ${APPLE} )
    # FLTK is not supported in 64-bit for Mac OS X. Until it is supported
    # we will force all applications to build in 32-bit. Libtiff must also
    # be compiled in 32-bit. 
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
  ENDIF( ${APPLE} )

ENDIF( ${WIN32} )


#=============================================================================
# Add Subdirectories
#=============================================================================
ADD_SUBDIRECTORY( src )

##############################################################################
#                           END of CMakeLists.txt
##############################################################################


