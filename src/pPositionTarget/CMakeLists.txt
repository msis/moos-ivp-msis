#--------------------------------------------------------
# The CMakeLists.txt for:                       pPositionTarget
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  pPositionTarget.cpp
  pPositionTarget_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pPositionTarget ${SRC})

TARGET_LINK_LIBRARIES(pPositionTarget
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

