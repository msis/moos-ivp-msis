#--------------------------------------------------------
# The CMakeLists.txt for:                       iEchoSounder
# Author(s):                              Équipe Ciscrea - Veni Vidi Vici
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  EchoSounder.cpp
  EchoSounder_Info.cpp
  EchoSounder_Tests.cpp
  main.cpp
)



ADD_EXECUTABLE(iEchoSounder ${SRC})

TARGET_LINK_LIBRARIES(iEchoSounder
   MOOS
   mbutil
   m
   pthread)

