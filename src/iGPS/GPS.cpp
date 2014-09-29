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
  AddMOOSVariable("Longitude", "", "GPS_LONG", dGPSPeriod);
  AddMOOSVariable("Latitude", "", "GPS_LAT", dGPSPeriod);
  AddMOOSVariable("Satellites", "", "GPS_SATS", dGPSPeriod);
  AddMOOSVariable("Altitude", "", "GPS_ALTITUDE", dGPSPeriod);
  AddMOOSVariable("Speed", "", "GPS_SPEED", dGPSPeriod);
  AddMOOSVariable("Heading", "", "GPS_HEADING", dGPSPeriod);
  AddMOOSVariable("Yaw", "", "GPS_YAW", dGPSPeriod);




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
  string sHeader = MOOSChomp(sNMEAString,",");
  bool bQuality = true;

  if (sHeader == "$GPRMC")
  {
    double dTimeMOOS = MOOSTime();
    // will only be used for speed and heading!
    string sTmp = MOOSChomp(sNMEAString,","); //UTC
    sTmp = MOOSChomp(sNMEAString, ","); // Validity
    if (sTmp == "V")
      bQuality = false;
    sTmp = MOOSChomp(sNMEAString, ","); // Lat
    sTmp = MOOSChomp(sNMEAString, ","); // N/S
    sTmp = MOOSChomp(sNMEAString, ","); // Long
    sTmp = MOOSChomp(sNMEAString, ","); // E/W
    
    sTmp = MOOSChomp(sNMEAString, ","); // Speed in knots
    double dSpeed = atof(sTmp.c_str());

    sTmp = MOOSChomp(sNMEAString, ","); // True course
    double dHeading = atof(sTmp.c_str());

    if (bQuality)
    {
      dSpeed = Knots2MPS(dSpeed);
      SetMOOSVar("Speed",dSpeed,dTimeMOOS);
      
      while(dHeading > 180) dHeading -= 360;
      while(dHeading < -180) dHeading += 360;
      double dYaw = -dHeading*M_PI/180.0;
      SetMOOSVar("Heading",dHeading,dTimeMOOS);
      SetMOOSVar("Yaw",dYaw,dTimeMOOS);
    }


  } else if (sHeader == "$GPGGA") {
    double dLat, dLong;
    double dTimeMOOS = MOOSTime();
    string sTmp = MOOSChomp(sNMEAString,",");

    // First time
    double dTime = atof(sTmp.c_str());

    // then Latitude
    sTmp = MOOSChomp(sNMEAString, ",");
    if (sTmp.size() == 0)
    {
      bQuality = false;
      MOOSTrace("NMEA message received with no Latitude.");
      return false;
    } else {
      dLat = atof(sTmp.c_str());
      sTmp = MOOSChomp(sNMEAString, ",");
      string sNS = sTmp;
      if (sNS == "S")
        dLat *= -1.0;
    }

    // then Longitude
    sTmp = MOOSChomp(sNMEAString, ",");
    if (sTmp.size() == 0)
    {
      bQuality = false;
      MOOSTrace("NMEA message received with no Longitude.");
      return false;
    } else {
      dLong = atof(sTmp.c_str());
      sTmp = MOOSChomp(sNMEAString, ",");
      string sEW = sTmp;
      if (sEW == "W")
        dLong *= -1.0;
    }

    // then GPS FIX Verification
    sTmp = MOOSChomp(sNMEAString, ",");
    int iFix = atoi(sTmp.c_str());
    if (iFix == 0)
      bQuality = false;

    // then number of stellites
    sTmp = MOOSChomp(sNMEAString, ",");
    int iSatellites = atoi(sTmp.c_str());
    if (iSatellites < 4)
      bQuality = false;

    // then Horizontal Dilution of Precision HDOP
    sTmp = MOOSChomp(sNMEAString, ",");
    double dHDOP = atof(sTmp.c_str());

    // then altitude above mean sea level
    sTmp = MOOSChomp(sNMEAString, ",");
    double dAltitude = atof(sTmp.c_str());
    sTmp = MOOSChomp(sNMEAString, ","); //removes M of meters.

    // then height of geoid above WGS84 ellipsoid
    sTmp = MOOSChomp(sNMEAString, ",");
    double dHeightGeoid = atof(sTmp.c_str());
    sTmp = MOOSChomp(sNMEAString, ","); //removes M of meters.

    // then time since last DGPS and DGPS ID
    // ignored

    // Conversion
    dLat = DMS2DecDeg(dLat);
    dLong = DMS2DecDeg(dLong);

    double dXLocal; // X coordinate in local
    double dYLocal; // Y coordinate in local
    double dNLocal; // Northing in local
    double dELocal; // Easting in local

    if (bQuality)
    {
      if (m_Geodesy.LatLong2LocalUTM(dLat,dLong,dNLocal,dELocal))
      {
        if (bQuality)
        {
          SetMOOSVar("N",dNLocal,dTimeMOOS);
          SetMOOSVar("E",dELocal,dTimeMOOS);
        }
      }
      if (m_Geodesy.LatLong2LocalUTM(dLat,dLong,dYLocal,dXLocal))
      {
        if (bQuality)
        {
          SetMOOSVar("X",dXLocal,dTimeMOOS);
          SetMOOSVar("Y",dYLocal,dTimeMOOS);
        }
      }
      SetMOOSVar("Longitude",dLong,dTimeMOOS);
      SetMOOSVar("Latitude",dLat,dTimeMOOS);
      SetMOOSVar("Satellites",iSatellites,dTimeMOOS);
      SetMOOSVar("Altitude",dAltitude,dTimeMOOS);
    }
  }
  return true;
}


double GPS::DMS2DecDeg(double dfVal)
{
  int nDeg = (int)(dfVal/100.0);

  double dfTmpDeg = (100.0*(dfVal/100.0-nDeg))/60.0;

  return  dfTmpDeg+nDeg;
}

double GPS::Knots2MPS(double speed)
{
  return speed*KNOTS2MPS;
}