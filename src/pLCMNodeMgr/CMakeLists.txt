#--------------------------------------------------------
# The CMakeLists.txt for:                       pLCMNodeMgr
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  LCMNodeMgr.cpp
  LCMNodeMgr_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pLCMNodeMgr ${SRC})

TARGET_LINK_LIBRARIES(pLCMNodeMgr
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread
   lcm)

