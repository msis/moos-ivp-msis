#--------------------------------------------------------
# The CMakeLists.txt for:                       iCISCREA
# Author(s):                              MSIS
#--------------------------------------------------------

SET(SRC
  M6dbus.cpp
  CISCREA.cpp
  CISCREA_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(iCISCREA ${SRC})

TARGET_LINK_LIBRARIES(iCISCREA
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   modbus
   pthread)

