#--------------------------------------------------------
# The CMakeLists.txt for:                       Images
# Author(s):                                  Simon Rohou
#--------------------------------------------------------

SET(SRC
  Ciscrea_Images.cpp
)

SET(HEADERS 
  Ciscrea_Images.h
) 

ADD_LIBRARY(Ciscrea_Images ${SRC})

TARGET_LINK_LIBRARIES(Ciscrea_Images
   mbutil
   behaviors
   geometry
   )
