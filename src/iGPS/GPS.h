/************************************************************/
/*    NAME: Mohamed Saad IBN SEDDIK                                              */
/*    ORGN: MIT                                             */
/*    FILE: GPS.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef GPS_HEADER
#define GPS_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"

class GPS : public CMOOSInstrument
{
 public:
   GPS();
   ~GPS();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();

   bool InitialiseSensor();

   bool GetData();
   bool PublishData();
   bool ParseNMEAString(std::string &s);

 private: // Configuration variables

 private: // State variables
   unsigned int m_iterations;
   double       m_timewarp;

   CMOOSGeodesy m_Geodesy;
};

#endif 
