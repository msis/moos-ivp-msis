#--------------------------------------------------------
# The CMakeLists.txt for:                       pBuoyDetection
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  pBuoyDetection.cpp
  pBuoyDetection_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pBuoyDetection ${SRC})

TARGET_LINK_LIBRARIES(pBuoyDetection
   MOOS
   mbutil
   m
   opencv_core
   opencv_imgproc
   opencv_highgui
   opencv_video
   opencv_legacy
   pthread)

