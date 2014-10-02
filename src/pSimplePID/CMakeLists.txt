#--------------------------------------------------------
# The CMakeLists.txt for:                       pSimplePID
# Author(s):                              Mohamed Saad IBN SEDDIK
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  SimplePID.cpp
  SimplePID_Info.cpp
  ScalarPID.cpp
  main.cpp
)



ADD_EXECUTABLE(pSimplePID ${SRC})

TARGET_LINK_LIBRARIES(pSimplePID
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

