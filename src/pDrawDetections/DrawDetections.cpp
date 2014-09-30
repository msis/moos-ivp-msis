/************************************************************/
/**    NAME:                                               **/
/**    FILE: DrawDetections.cpp                                  **/
/**    DATE:                                               **/
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "DrawDetections.h"

using namespace std;

//---------------------------------------------------------
// Constructor

DrawDetections::DrawDetections()
{
  m_nav_x = 0.;
  m_nav_y = 0.;
  m_iteration = 0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool DrawDetections::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++)
  {
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
    
    if(key == "NAV_X") 
      m_nav_x = msg.GetDouble();

    else if(key == "NAV_Y") 
      m_nav_y = msg.GetDouble();

    else if(key == "PIPE_DETECTION")
    {
      if(msg.GetString() != "false")
      {
        m_iteration ++;
        XYCircle circle;
        circle.setX(m_nav_x);
        circle.setY(m_nav_y);
        
        ostringstream strs;
        strs << "d" << m_iteration;
        circle.set_label(strs.str());
        circle.set_active("true");
        circle.set_color("label", "orange");
        circle.set_color("edge", "orange");
        circle.set_color("fill", "orange");
        circle.setRad(3);
        circle.set_vertex_size(2);
        circle.set_edge_size(1);
        circle.set_transparency(0.3);
        
        Notify("VIEW_CIRCLE", circle.get_spec());
      }
    }

    else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool DrawDetections::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool DrawDetections::Iterate()
{
  AppCastingMOOSApp::Iterate();
  
  AppCastingMOOSApp::PostReport();
  return true;
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool DrawDetections::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++)
  {
    string orig  = *p;
    string line  = *p;
    string param = toupper(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    
    /*if(param == "LOGS_FOLDER")
    {
      handled = true;
      m_logs_folder = value;
    }*/
    
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void DrawDetections::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("PIPE_DETECTION", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool DrawDetections::buildReport() 
{
  return(true);
}
