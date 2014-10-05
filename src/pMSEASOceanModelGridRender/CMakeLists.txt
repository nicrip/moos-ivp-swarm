#--------------------------------------------------------
# The CMakeLists.txt for:                       pMSEASOceanModelGridRender
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

include_directories (${Octave_INCLUDE_DIR})

include_directories (${Octave_INCLUDE_DIR}/octave)

SET(SRC
  MSEASOceanModelGridRender.cpp
  MSEASOceanModelGridRender_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pMSEASOceanModelGridRender ${SRC})

TARGET_LINK_LIBRARIES(pMSEASOceanModelGridRender
   ${Octave_LIBRARIES}
   ${MOOS_LIBRARIES}
   MOOSGeodesy
   proj
   geometry
   mbutil
   m
   pthread)

