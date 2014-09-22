/************************************************************/
/**    NAME:                                               **/
/**    FILE: SauceCSV.cpp                                  **/
/**    DATE:                                               **/
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "SauceCSV.h"

using namespace std;

/*
  Each team will produce a log file of the mission within around 10 minutes of the end of
  the run (unless additional available time explicitly stated in the rules, e.g. in Task 2). The
  format of the log file will be a comma separated ASCII file of the format:
  (TIME, LAT, LONG, DEPTH, ACTION).
  Where:
  TIME is the time in seconds from the beginning of the mission;
  LAT is the latitude in decimal degrees. E.g. 44.2456;
  LONG is the longitude in decimal degrees;
  DEPTH is the depth of the vehicle in meters;
  ACTION is a string communicating the action/state the vehicle is performing.
  Logged data will be plotted by plotting routines written by the organising committee. This
  will be used to score the log file. For Task 2 and Task 4 additional data about the
  mapping are required while for Task 3 the additional file with light on/off data and the
  sent-received acoustic messages have to be provided.
*/

//---------------------------------------------------------
// Constructor

SauceCSV::SauceCSV()
{
  m_nav_x = 0.;
  m_nav_y = 0.;
  m_depth = 0.;
  m_dbuptime = 0.;
  m_mode = "?";
  m_logs_folder = "./logs/";
  m_logs_lines = 0;
  
  ostringstream s;
  s << setprecision(10) << MOOSTime() << ".csv";
  m_logs_file_name = s.str();
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool SauceCSV::OnNewMail(MOOSMSG_LIST &NewMail)
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

    else if(key == "DEPTH") 
      m_depth = msg.GetDouble();

    else if(key == "DB_UPTIME") 
      m_dbuptime = msg.GetDouble();

    else if(key == "MODE") 
      m_mode = msg.GetString();

    else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SauceCSV::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool SauceCSV::Iterate()
{
  AppCastingMOOSApp::Iterate();
  
  ofstream logs_file((char*)(m_logs_folder + "/" + m_logs_file_name).c_str(), ios::out | ios::app); // Open in writing mode

	if(logs_file)
	{
    ostringstream s;
    s << m_dbuptime << setprecision(10) << "," <<
          m_nav_x << setprecision(8) << "," << 
          m_nav_y << setprecision(8) << "," << 
          m_depth << setprecision(8) << "," << 
          m_mode << endl;
		logs_file << s.str();
		logs_file.close();
    m_logs_lines ++;
	}
  
  AppCastingMOOSApp::PostReport();
  return true;
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool SauceCSV::OnStartUp()
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
    
    if(param == "LOGS_FOLDER")
    {
      handled = true;
      m_logs_folder = value;
    }
    
    if(param == "LOGS_FILE_NAME")
    {
      handled = true;
      m_logs_file_name = value;
    }
    
    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void SauceCSV::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  Register("DEPTH", 0);
  Register("DB_UPTIME", 0);
  Register("MODE", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool SauceCSV::buildReport() 
{
  m_msgs << "Logs file name: " << m_logs_folder << "/" << m_logs_file_name << endl;
  m_msgs << "Logs lines: " << m_logs_lines << endl;
  return(true);
}
