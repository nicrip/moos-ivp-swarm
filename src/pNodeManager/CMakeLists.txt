#--------------------------------------------------------
# The CMakeLists.txt for:                       pNodeManager
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

include_directories (${Octave_INCLUDE_DIR})

include_directories (${Octave_INCLUDE_DIR}/octave)

SET(SRC
  NodeManager.cpp
  NodeManager_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pNodeManager ${SRC})

TARGET_LINK_LIBRARIES(pNodeManager
   ${Octave_LIBRARIES}
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

