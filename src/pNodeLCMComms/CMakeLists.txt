#--------------------------------------------------------
# The CMakeLists.txt for:                       pNodeLCMComms
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  NodeLCMComms.cpp
  NodeLCMComms_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pNodeLCMComms ${SRC})

TARGET_LINK_LIBRARIES(pNodeLCMComms
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

