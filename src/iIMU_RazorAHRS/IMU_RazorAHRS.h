/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: IMU_RazorAHRS.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef IMU_RazorAHRS_HEADER
#define IMU_RazorAHRS_HEADER


#include "MOOS/libMOOS/MOOSLib.h"

#include <iostream>   // cout()
#include <iomanip>    // setprecision() etc.
#include <stdexcept>  // runtime_error
#include <cstdio>     // getchar()
#include "../../3rdParties/razor-9dof-ahrs/C++/RazorAHRS.h"

using namespace std;
using namespace std::placeholders;


class IMU_RazorAHRS : public CMOOSApp
{
 public:
   IMU_RazorAHRS();
   ~IMU_RazorAHRS();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();

   bool InitRazorAHRS();
   void OnRazorError(const string &msg);
   void OnRazorData(const float data[]);


 private: // Configuration variables

 private: // State variables
   double       m_timewarp;

   RazorAHRS *m_razor;

   double m_gyro_x,m_gyro_y,m_gyro_z;
   double m_mag_x,m_mag_y,m_mag_z;
   double m_acc_x,m_acc_y,m_acc_z;
   double m_roll, m_pitch, m_yaw;

   string m_port_name;
   RazorAHRS::Mode m_razor_mode;
};

#endif 
