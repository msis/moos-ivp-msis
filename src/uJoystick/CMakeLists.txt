#--------------------------------------------------------
# The CMakeLists.txt for:                       uJoystick
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  Joystick.cpp
  Joystick_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(uJoystick ${SRC})

TARGET_LINK_LIBRARIES(uJoystick
   MOOS
   mbutil
   m
   pthread)

