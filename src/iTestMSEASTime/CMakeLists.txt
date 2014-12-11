#--------------------------------------------------------
# The CMakeLists.txt for:                       iTestMSEASTime
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

include_directories (${Octave_INCLUDE_DIR})

include_directories (${Octave_INCLUDE_DIR}/octave)

SET(SRC
  TestMSEASTime.cpp
  TestMSEASTime_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(iTestMSEASTime ${SRC})

TARGET_LINK_LIBRARIES(iTestMSEASTime
   ${Octave_LIBRARIES}
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread
   contacts)

