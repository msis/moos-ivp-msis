/************************************************************/
/*    NAME: Mohamed Saad IBN SEDDIK                                              */
/*    ORGN: MIT                                             */
/*    FILE: SimplePID.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef SimplePID_HEADER
#define SimplePID_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "AngleUtils.h"
#include "ScalarPID.h"

class SimplePID : public CMOOSApp
{
 public:
   SimplePID();
   ~SimplePID();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();

 private: // Configuration variables

 private: // State variables
   unsigned int m_iterations;
   double       m_timewarp;

   ScalarPID m_pid;

   bool m_active;
   bool m_angular;

   double m_max_output;

};

#endif 
