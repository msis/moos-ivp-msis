#--------------------------------------------------------
# The CMakeLists.txt for:                       pDrawDetections
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  DrawDetections.cpp
  DrawDetections_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pDrawDetections ${SRC})

TARGET_LINK_LIBRARIES(pDrawDetections
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread
   geometry
   ivpbuild 
   logic 
   ivpcore 
   bhvutil 
   mbutil 
   geometry )

