#--------------------------------------------------------
# The CMakeLists.txt for:                       pSauceCSV
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  SauceCSV.cpp
  SauceCSV_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pSauceCSV ${SRC})

TARGET_LINK_LIBRARIES(pSauceCSV
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

