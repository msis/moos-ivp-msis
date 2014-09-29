#--------------------------------------------------------
# The CMakeLists.txt for:                       pControl
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  iheadingcontroller.cpp
  pControl.cpp
  pControl_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pControl ${SRC})

TARGET_LINK_LIBRARIES(pControl
   MOOS
   mbutil
   m
   opencv_core
   opencv_imgproc
   opencv_highgui
   opencv_video
   opencv_legacy
   pthread)

