#--------------------------------------------------------
# The CMakeLists.txt for:                       pSwarmManager
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

include_directories (${Octave_INCLUDE_DIR})

include_directories (${Octave_INCLUDE_DIR}/octave)

SET(SRC
  SwarmManager.cpp
  SwarmManager_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pSwarmManager ${SRC})

TARGET_LINK_LIBRARIES(pSwarmManager
   ${Octave_LIBRARIES}
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

