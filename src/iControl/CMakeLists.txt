#--------------------------------------------------------
# The CMakeLists.txt for:                       iControl
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  iheadingcontroller.cpp
  iControl.cpp
  iControl_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(iControl ${SRC})

TARGET_LINK_LIBRARIES(iControl
   MOOS
   mbutil
   m
   opencv_core
   opencv_imgproc
   opencv_highgui
   opencv_video
   opencv_legacy
   pthread)

