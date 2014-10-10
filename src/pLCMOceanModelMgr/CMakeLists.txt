#--------------------------------------------------------
# The CMakeLists.txt for:                       pLCMOceanModelMgr
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

include_directories (${Octave_INCLUDE_DIR})

include_directories (${Octave_INCLUDE_DIR}/octave)

SET(SRC
  LCMOceanModelMgr.cpp
  LCMOceanModelMgr_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pLCMOceanModelMgr ${SRC})

TARGET_LINK_LIBRARIES(pLCMOceanModelMgr
   ${Octave_LIBRARIES}
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread
   lcm)

