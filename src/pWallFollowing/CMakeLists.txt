#--------------------------------------------------------
# The CMakeLists.txt for:                       pWallFollowing
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  WallFollowing.cpp
  WallFollowing_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pWallFollowing ${SRC})

TARGET_LINK_LIBRARIES(pWallFollowing
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread
   #geometry
   opencv_core
   opencv_imgproc
   opencv_highgui
   opencv_ml
   opencv_video
   opencv_features2d
   opencv_calib3d
   opencv_objdetect
   #opencv_contrib
   opencv_legacy
   opencv_flann
   Ciscrea_Images)

