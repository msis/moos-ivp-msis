#--------------------------------------------------------
# The CMakeLists.txt for:                       uDeReCISCREA
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  DeReCISCREA.cpp
  DeReCISCREA_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(uDeReCISCREA ${SRC})

TARGET_LINK_LIBRARIES(uDeReCISCREA
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

