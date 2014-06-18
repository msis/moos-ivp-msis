#--------------------------------------------------------
# The CMakeLists.txt for:                       iIMU_RazorAHRS
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(CMAKE_CXX_FLAGS "-D_REENTRANT=1 -std=c++11")

SET(SRC
  ../../3rdParties/razor-9dof-ahrs/C++/RazorAHRS.cpp
  IMU_RazorAHRS.cpp
  IMU_RazorAHRS_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(iIMU_RazorAHRS ${SRC})

TARGET_LINK_LIBRARIES(iIMU_RazorAHRS
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

