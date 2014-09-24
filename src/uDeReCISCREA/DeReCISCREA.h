/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: DeReCISCREA.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef DeReCISCREA_HEADER
#define DeReCISCREA_HEADER

#include "math.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class DeReCISCREA : public AppCastingMOOSApp
{
 public:
   DeReCISCREA();
   ~DeReCISCREA() {};

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

 private: // Configuration variables
 	double m_start_x,m_start_y;
 	double m_speed_factor;

 private: // State variables
 	double m_nav_x, m_nav_y;
 	double m_nav_heading;
 	double m_desired_thrust,m_desired_speed;
 	double m_time;

};

#endif 
