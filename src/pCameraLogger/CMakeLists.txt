#--------------------------------------------------------
# The CMakeLists.txt for:                       pCameraLogger
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  pCameraLogger.cpp
  pCameraLogger_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pCameraLogger ${SRC})

TARGET_LINK_LIBRARIES(pCameraLogger
   MOOS
   mbutil
   m
   opencv_core
   opencv_imgproc
   opencv_highgui
   opencv_video
   pthread
   # Our libraries
   Ciscrea_Images)

