#--------------------------------------------------------
# The CMakeLists.txt for:                       pLocalizationSonar
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  LocalizationSonar.cpp
  LocalizationSonar_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pLocalizationSonar ${SRC})

TARGET_LINK_LIBRARIES(pLocalizationSonar
   MOOS
   mbutil
   m
   opencv_core
   opencv_imgproc
   opencv_highgui
   opencv_video
   opencv_legacy
   pthread)

