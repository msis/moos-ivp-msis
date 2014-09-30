#--------------------------------------------------------
# The CMakeLists.txt for:                       iGPS
# Author(s):                              Mohamed Saad IBN SEDDIK
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  GPS.cpp
  GPS_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(iGPS ${SRC})

TARGET_LINK_LIBRARIES(iGPS
   ${MOOS_LIBRARIES}
   ${MOOSGeodesy_LIBRARIES}
   mbutil
   m
   pthread)

