/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: DeReCISCREA.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "DeReCISCREA.h"

using namespace std;

//---------------------------------------------------------
// Constructor

DeReCISCREA::DeReCISCREA():
  m_start_x(0),m_start_y(0),
  m_speed_factor(0.0115),
  m_nav_x(0),m_nav_y(0),
  m_nav_heading(0),
  m_desired_thrust(0),m_desired_speed(0)
{
  m_time = MOOSTime();
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool DeReCISCREA::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(key == "DESIRED_THRUST") 
      m_desired_thrust = msg.GetDouble();
    else if (key == "NAV_HEADING")
      m_nav_heading = msg.GetDouble();
    else if (key == "NAV_X")
      m_nav_x = msg.GetDouble();
    else if (key == "NAV_Y")
      m_nav_y = msg.GetDouble();
    else if (key == "DESIRED_SPEED")
      m_desired_speed = msg.GetDouble();
    else if (key == "SPEED_FACTOR")
      m_speed_factor = msg.GetDouble();

    else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool DeReCISCREA::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool DeReCISCREA::Iterate()
{
  AppCastingMOOSApp::Iterate();
  
  double new_time = MOOSTime();
  double dt = new_time - m_time;

  m_nav_x += cos(MOOSDeg2Rad(m_nav_heading))*m_speed_factor*m_desired_thrust*dt;
  m_nav_y += sin(MOOSDeg2Rad(m_nav_heading))*m_speed_factor*m_desired_thrust*dt;

  m_time = new_time;

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool DeReCISCREA::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "SPEED_FACTOR") {
      m_speed_factor = atof(value.c_str());
      handled = true;
    }
    else if(param == "START_X") {
      m_start_x = atof(value.c_str());
      handled = true;
    }
    else if(param == "START_Y") {
      m_start_y = atof(value.c_str());
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void DeReCISCREA::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
  Register("DESIRED_THRUST",0);
  Register("DESIRED_SPEED",0);
  Register("NAV_HEADING",0);
  Register("SPEED_FACTOR",0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool DeReCISCREA::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << "File:                                        \n";
  m_msgs << "============================================ \n";

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one" << "two" << "three" << "four";
  m_msgs << actab.getFormattedString();

  return(true);
}




