##############################################################################
# FILE:        moos-ivp-extend/src/CMakeLists.txt
# DATE:        2010/09/07
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-extend source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_*)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

#============================================================================
# List the subdirectories to build...
#============================================================================
find_package(Octave REQUIRED)
include_directories(${Octave_INCLUDE_DIRS})
ADD_SUBDIRECTORY(lcmtypes)
ADD_SUBDIRECTORY(iMSEASOceanModel)
ADD_SUBDIRECTORY(iMSEASOceanModelDirect)
ADD_SUBDIRECTORY(pMSEASOceanModelTester)
ADD_SUBDIRECTORY(pMSEASOceanModelGridRender)
ADD_SUBDIRECTORY(pNodeManager)
ADD_SUBDIRECTORY(pSwarmManager)
#ADD_SUBDIRECTORY(pNodeLCMComms)
#ADD_SUBDIRECTORY(pShoreLCMComms)
#ADD_SUBDIRECTORY(pLCMOceanModelMgr)
#ADD_SUBDIRECTORY(pLCMNodeMgr)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

