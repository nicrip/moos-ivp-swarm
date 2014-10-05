#--------------------------------------------------------
# The CMakeLists.txt for:                       pShoreLCMComms
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  ShoreLCMComms.cpp
  ShoreLCMComms_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pShoreLCMComms ${SRC})

TARGET_LINK_LIBRARIES(pShoreLCMComms
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

