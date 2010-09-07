#--------------------------------------------------------
# The CMakeLists.txt for:                         AOF_Gaussian2
# Author(s):                                Christian Convey
#--------------------------------------------------------

# You may need to adjust these paths, depending on where you're moos-ivp source
# code resides...

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF( ${WIN32} )
  # Windows does not have symbolic links. To handle this, subversion creates 
  # a text file that represents the symbolic link with the destination of the 
  # link inside the file. Here we extract the destination of the MOOS directory 
  # link and use it set a variable.
  FIND_FILE(MOOS_LINK MOOS ../../../moos-ivp/)
  FILE(READ ${MOOS_LINK} MOOS_LINK_DIR OFFSET 5)

  # For Windows, use the following paths
  GET_FILENAME_COMPONENT(MOOS_BASE_DIR_ABSOLUTE 
	  ../../../moos-ivp/${MOOS_LINK_DIR} ABSOLUTE)

ELSE( ${WIN32} )
  # For Linux and Apple, use the following paths:
  GET_FILENAME_COMPONENT(MOOS_BASE_DIR_ABSOLUTE ../../../moos-ivp/MOOS    
	  ABSOLUTE)
ENDIF( ${WIN32} )

# Use the following paths for all systems:
GET_FILENAME_COMPONENT(IVP_SRC_DIR_ABSOLUTE   ../../../moos-ivp/ivp/src 
	  ABSOLUTE)
GET_FILENAME_COMPONENT(IVP_LIB_DIR_ABSOLUTE ../../../moos-ivp/lib ABSOLUTE)

#===============================================================================
# Add the directories containing MOOS's header files to the include path, and
# add the directory containing MOOS's library files to the library path.
#===============================================================================

# Confirm that MOOS_BASE_DIR seems to be set to a valid value...
IF(NOT EXISTS ${MOOS_BASE_DIR_ABSOLUTE}/Core)
   MESSAGE(FATAL_ERROR 
      "The CMake variable MOOS_BASE_DIR_ABSOLUTE has the value "
      "'${MOOS_BASE_DIR_ABSOLUTE}'.\n\n"
      "That directory doesn't have a 'Core' subdirectory, which means it's "
      "definitely not the base directory of a MOOS source tree."
      "\n\n"
      "You need to set the CMake variable 'MOOS_BASE_DIR' to a different value."
)
ENDIF(NOT EXISTS ${MOOS_BASE_DIR_ABSOLUTE}/Core)

LINK_DIRECTORIES(${MOOS_BASE_DIR_ABSOLUTE}/MOOSBin)

# We're listing all of MOOS's header directories here, even though this 
# particular application doesn't need all of them.  We do this so that this
# CMake code can be used in any MOOS application...
INCLUDE_DIRECTORIES(
   ${MOOS_BASE_DIR_ABSOLUTE}/Essentials
   ${MOOS_BASE_DIR_ABSOLUTE}/Core
   ${MOOS_BASE_DIR_ABSOLUTE}/Essentials/MOOSUtilityLib
   ${MOOS_BASE_DIR_ABSOLUTE}/Core/MOOSLIB
   ${MOOS_BASE_DIR_ABSOLUTE}/Core/MOOSGenLib
   )

#===============================================================================
# Add the directories containing IvP's header files to the include path, and
# add the directoies containing IvP's library files to the library path.
#===============================================================================

# Confirm that MOOS_BASE_DIR seems to be set to a valid value...
IF(NOT EXISTS ${IVP_SRC_DIR_ABSOLUTE}/pEchoVar)
   MESSAGE(FATAL_ERROR 
      "The CMake variable IVP_SRC_DIR_ABSOLUTE has the value "
      "'${IVP_SRC_DIR_ABSOLUTE}'.\n\n"
      "That directory doesn't have a 'pEchoVar' subdirectory, which means it's "
      "definitely not the base directory of a MOOS source tree."
      "\n\n"
      "You need to set the CMake variable 'IVP_SRC_DIR_ABSOLUTE' to a different value."
)
ENDIF(NOT EXISTS ${IVP_SRC_DIR_ABSOLUTE}/pEchoVar)

# We're listing all of IvP's library directories here, even though this 
# particular application doesn't need all of them.  We do this so that this
# CMake code can be used in any IvP application...
SET(IVP_LIB_DIRS
   ${IVP_SRC_DIR_ABSOLUTE}/lib_anrp_util
   ${IVP_SRC_DIR_ABSOLUTE}/lib_behaviors
   ${IVP_SRC_DIR_ABSOLUTE}/lib_behaviors-marine
   ${IVP_SRC_DIR_ABSOLUTE}/lib_bhvutil
   ${IVP_SRC_DIR_ABSOLUTE}/lib_fltk-1.1.7
   ${IVP_SRC_DIR_ABSOLUTE}/lib_genutil
   ${IVP_SRC_DIR_ABSOLUTE}/lib_geometry
   ${IVP_SRC_DIR_ABSOLUTE}/lib_helmivp
   ${IVP_SRC_DIR_ABSOLUTE}/lib_ipfview
   ${IVP_SRC_DIR_ABSOLUTE}/lib_ivpbuild
   ${IVP_SRC_DIR_ABSOLUTE}/lib_ivpcore
   ${IVP_SRC_DIR_ABSOLUTE}/lib_logic
   ${IVP_SRC_DIR_ABSOLUTE}/lib_logutils
   ${IVP_SRC_DIR_ABSOLUTE}/lib_marineview
   ${IVP_SRC_DIR_ABSOLUTE}/lib_mbutil
   ${IVP_SRC_DIR_ABSOLUTE}/lib_navplot
   ${IVP_SRC_DIR_ABSOLUTE}/lib_newmat10D
   )

# IvP keeps the source code, header files, and built library files all in the
# same directory for each of its libraries...
LINK_DIRECTORIES(${IVP_LIB_DIRS} ${IVP_LIB_DIR_ABSOLUTE} )
INCLUDE_DIRECTORIES(${IVP_LIB_DIRS})

#===============================================================================

ADD_LIBRARY(AOF_Gaussian2 SHARED 
   AOF_Gaussian2.cpp)

# Set System Specific Libraries
if (${WIN32})
  # Windows Libraries
  SET(SYSTEM_LIBS
      )
else (${WIN32})
  # Linux and Apple Libraries
  SET(SYSTEM_LIBS
      m )
endif (${WIN32})

TARGET_LINK_LIBRARIES(AOF_Gaussian2
   ivpbuild 
#    logic 
   ivpcore 
   bhvutil 
   mbutil 
   geometry 
   ${SYSTEM_LIBS} )
