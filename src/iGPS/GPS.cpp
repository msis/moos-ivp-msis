/************************************************************/
/*    NAME: Mohamed Saad IBN SEDDIK                                              */
/*    ORGN: MIT                                             */
/*    FILE: GPS.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "GPS.h"

using namespace std;

//---------------------------------------------------------
// Constructor

GPS::GPS()
{
  m_iterations = 0;
  m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

GPS::~GPS()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool GPS::OnNewMail(MOOSMSG_LIST &NewMail)
{
//   MOOSMSG_LIST::iterator p;
   
//   for(p=NewMail.begin(); p!=NewMail.end(); p++) {
//     CMOOSMsg &msg = *p;

// #if 0 // Keep these around just for template
//     string key   = msg.GetKey();
//     string comm  = msg.GetCommunity();
//     double dval  = msg.GetDouble();
//     string sval  = msg.GetString(); 
//     string msrc  = msg.GetSource();
//     double mtime = msg.GetTime();
//     bool   mdbl  = msg.IsDouble();
//     bool   mstr  = msg.IsString();
// #endif
//    }
	
   // return(true);
   return UpdateMOOSVariables(NewMail);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool GPS::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);
	
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool GPS::Iterate()
{
  m_iterations++; 

  if(GetData())
    PublishData();

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool GPS::OnStartUp()
{
  CMOOSInstrument::OnStartUp();

  double dLatOrigin;
  if (!m_MissionReader.GetValue("LatOrigin",dLatOrigin))
  {
    MOOSTrace("LatOrigin not set!!!");
    return false;
  }

  double dLongOrigin;
  if (!m_MissionReader.GetValue("LongOrigin",dLongOrigin))
  {
    MOOSTrace("LangOrigin not set!!!");
    return false;
  }

  if (!m_Geodesy.Initialise(dLatOrigin,dLongOrigin))
  {
    MOOSTrace("Geodesy initialisation failed!!!");
    return false;
  }
  
  int max_retries = 5;
  double dGPSPeriod = 1.0;

  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);
      
      if(param == "GPS_PERIOD") {
        dGPSPeriod = atof(value.c_str());
      }
      else if(param == "MAX_RETRIES") {
        max_retries = atoi(value.c_str());
      }
    }
  }
  
  m_timewarp = GetMOOSTimeWarp();

  AddMOOSVariable("X", "", "GPS_X", dGPSPeriod);
  AddMOOSVariable("Y", "", "GPS_Y", dGPSPeriod);
  AddMOOSVariable("N", "", "GPS_N", dGPSPeriod);
  AddMOOSVariable("E", "", "GPS_E", dGPSPeriod);
  AddMOOSVariable("Raw", "", "GPS_RAW", dGPSPeriod);

  RegisterMOOSVariables();
  RegisterVariables();


  if (!SetupPort()) 
  {
    return false;
  }
  if (!(InitialiseSensorN(max_retries,"GPS")))
  {
    return false;
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void GPS::RegisterVariables()
{
  // m_Comms.Register("FOOBAR", 0);
}

bool GPS::InitialiseSensor()
{
  MOOSPause(1000);
  if (m_Port.Flush()==-1)
  {
    return false;
  } else return true;
}

bool GPS::GetData()
{
  string sMessage;
  double dTime;

  if(m_Port.IsStreaming())
  {
    if (!m_Port.GetLatest(sMessage,dTime))
    {
      return false;
    }
  } else {
    if (!m_Port.GetTelegram(sMessage,0.5))
    {
      return false;
    }
  }
  if (PublishRaw())
  {
    SetMOOSVar("Raw",sMessage,MOOSTime());
  }

  return ParseNMEAString(sMessage);
}

bool GPS::PublishData()
{
  return PublishFreshMOOSVariables();
}

bool GPS::ParseNMEAString(string &sNMEAString)
{
  if(!DoNMEACheckSum(sNMEAString))
  {
    MOOSDebugWrite("GPS NMEA checksum failed!");
    return false;
  }
  string sCopy = sNMEAString;
  string sMessage = MOOSChomp(sNMEAString,",");

  return true;
}