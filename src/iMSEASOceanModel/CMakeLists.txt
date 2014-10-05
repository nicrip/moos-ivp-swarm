#--------------------------------------------------------
# The CMakeLists.txt for:                       iMSEASOceanModel
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

include_directories (${Octave_INCLUDE_DIR})

include_directories (${Octave_INCLUDE_DIR}/octave)

SET(SRC
  MSEASOceanModel.cpp
  MSEASOceanModel_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(iMSEASOceanModel ${SRC})

TARGET_LINK_LIBRARIES(iMSEASOceanModel
   ${Octave_LIBRARIES}
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

