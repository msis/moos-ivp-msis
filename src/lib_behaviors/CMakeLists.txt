##############################################################################
# FILE:        moos-ivp-extend/src/lib_behaviors/CMakeLists.txt
# DATE:        2010/09/07
# DESCRIPTION: CMakeLists.txt file
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_COMMON_LIBRARY_DIRS ./lib_common/lib_*)
FILE(GLOB LOCAL_BHV_LIBRARY_DIRS ./lib_behaviors/lib_*)
INCLUDE_DIRECTORIES(${LOCAL_COMMON_LIBRARY_DIRS} ${LOCAL_BHV_LIBRARY_DIRS})

#============================================================================
# List the subdirectories to build...
#============================================================================

# Behavior libs
ADD_SUBDIRECTORY(WallFollowing_EchoSounder)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

