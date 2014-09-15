/************************************************************/
/*    NAME: MSIS                                              */
/*    ORGN: MIT                                             */
/*    FILE: CISCREA.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "CISCREA.h"

using namespace std;

//---------------------------------------------------------
// Constructor

CISCREA::CISCREA()
{
  m_serial_port = "/dev/ttyUSB0";

  m_frri_lo_dz = 0;
  m_reri_lo_dz = 0;
  m_frle_lo_dz = 0;
  m_rele_lo_dz = 0;
  m_vert_lo_dz = 0;

  m_frri_up_dz = 0;
  m_reri_up_dz = 0;
  m_frle_up_dz = 0;
  m_rele_up_dz = 0;
  m_vert_up_dz = 0;

  m_frri_sens = 1;
  m_reri_sens = 1;
  m_frle_sens = 1;
  m_rele_sens = 1;
  m_vert_sens = 1;
  
  m_desired_thrust=0;
  m_desired_rudder=0;
  m_desired_slide=0;
  m_desired_elevator=0;
  m_front_lights=0;

  m_depth_offset=0;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool CISCREA::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    double dval  = msg.GetDouble();

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
      m_desired_thrust = dval;
    else if (key == "DESIRED_SLIDE")
      m_desired_slide = dval;
    else if (key == "DESIRED_RUDDER")
      m_desired_rudder = dval;
    else if (key == "DESIRED_ELEVATOR")
      m_desired_elevator = dval;
    else if (key == "FRONT_LIGHTS")
      m_front_lights = dval;


     else if(key != "APPCAST_REQ") // handle by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CISCREA::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool CISCREA::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!

  m_ciscrea->updateAll();
  Notify("DEPTH",m_ciscrea->getDepth()/100.0 + m_depth_offset);
  Notify("CURRENT_BATT1",m_ciscrea->getI1());
  Notify("CURRENT_BATT2",m_ciscrea->getI2());
  Notify("VOLTAGE_BATT1",m_ciscrea->getU1()/1000.0);
  Notify("VOLTAGE_BATT2",m_ciscrea->getU2()/1000.0);
  Notify("CONSUMPTION_BATT1",m_ciscrea->getCapBat1());
  Notify("CONSUMPTION_BATT2",m_ciscrea->getCapBat2());
  Notify("HEADING",m_ciscrea->getDirec()/10.0);

  sendCommands();


  AppCastingMOOSApp::PostReport();
  return(true);
}

void CISCREA::sendCommands()
{
  int frri_val=0,frle_val=0,rele_val=0,reri_val=0;

  // thrust
  frri_val = (-1)*m_frri_sens*m_desired_thrust;
  reri_val = (+1)*m_reri_sens*m_desired_thrust;
  rele_val = (+1)*m_rele_sens*m_desired_thrust;
  frle_val = (-1)*m_frle_sens*m_desired_thrust;

  // slide
  frri_val += (-1)*m_frri_sens*m_desired_slide;
  reri_val += (-1)*m_reri_sens*m_desired_slide;
  rele_val += (+1)*m_rele_sens*m_desired_slide;
  frle_val += (+1)*m_frle_sens*m_desired_slide;

  // rotation
  frri_val += (-1)*m_frri_sens*m_desired_rudder;
  reri_val += (+1)*m_reri_sens*m_desired_rudder;
  rele_val += (-1)*m_rele_sens*m_desired_rudder;
  frle_val += (+1)*m_frle_sens*m_desired_rudder;

  // depth
  int vert_val = m_vert_sens*m_desired_elevator;

  // deadzone corrections
  if (frri_val>=0)
    frri_val+=m_frri_up_dz;
  else
    frri_val-=m_frri_lo_dz;
  if (frle_val>=0)
    frle_val+=m_frle_up_dz;
  else
    frle_val-=m_frle_lo_dz;
  if (reri_val>=0)
    reri_val+=m_reri_up_dz;
  else
    reri_val-=m_reri_lo_dz;
  if (rele_val>=0)
    rele_val+=m_rele_up_dz;
  else
    rele_val-=m_rele_lo_dz;
  if (vert_val>=0)
    vert_val+=m_vert_up_dz;
  else
    vert_val-=m_vert_lo_dz;

  m_ciscrea->setPropFrRi(frri_val);
  m_ciscrea->setPropReRi(reri_val);
  m_ciscrea->setPropReLe(rele_val);
  m_ciscrea->setPropFrLe(frle_val);
  m_ciscrea->setPropVert(vert_val);
  m_ciscrea->updatePropulsors();
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool CISCREA::OnStartUp()
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
    if(param == "SERIAL_PORT") {
      m_serial_port = value;
      handled = true;
    }
    else if(param == "MAX_MODBUS_RETRIES") {
      m_max_modbus_retries = atoi(value.c_str());
      handled = true;
    }
    else if(param == "MAX_TURNON_RETRIES") {
      m_max_turn_on_retries = atoi(value.c_str());
      handled = true;
    }
    else if(param == "FRRI_LO_DZ") {
      m_frri_lo_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "RERI_LO_DZ") {
      m_reri_lo_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "RELE_LO_DZ") {
      m_rele_lo_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "FRLE_LO_DZ") {
      m_frle_lo_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "VERT_LO_DZ") {
      m_vert_lo_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "FRRI_UP_DZ") {
      m_frri_up_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "RERI_UP_DZ") {
      m_reri_up_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "RELE_UP_DZ") {
      m_rele_up_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "FRLE_UP_DZ") {
      m_frle_up_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "VERT_UP_DZ") {
      m_vert_up_dz = atoi(value.c_str());
      handled = true;
    }
    else if(param == "FRRI_SENS") {
      m_frri_sens = atoi(value.c_str());
      handled = true;
    }
    else if(param == "RERI_SENS") {
      m_reri_sens = atoi(value.c_str());
      handled = true;
    }
    else if(param == "RELE_SENS") {
      m_rele_sens = atoi(value.c_str());
      handled = true;
    }
    else if(param == "FRLE_SENS") {
      m_frle_sens = atoi(value.c_str());
      handled = true;
    }
    else if(param == "VERT_SENS") {
      m_vert_sens = atoi(value.c_str());
      handled = true;
    }
    else if(param == "DEPTH_OFFSET") {
      m_depth_offset = atoi(value.c_str());
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  Notify("CISCREA_ON", "false");
  registerVariables();	
  // return(true);
  return connectToAUV();
}

//---------------------------------------------------------
// Procedure: registerVariables

void CISCREA::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
  Register("DESIRED_THRUST",0);
  Register("DESIRED_SLIDE",0);
  Register("DESIRED_RUDDER",0);
  Register("DESIRED_ELEVATOR",0);
  Register("FRONT_LIGHTS",0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool CISCREA::buildReport() 
{
  m_msgs << "============================================ \n";
  m_msgs << "iCISCREA report\n";
  m_msgs << "============================================ \n";

  ACTable actab(3);
  actab << "Variable | Value | Unit";
  actab.addHeaderLines();
  actab << "DEPTH" << m_ciscrea->getDepth()/100.0 + m_depth_offset << "m";
  actab << "CURRENT_BATT1" << m_ciscrea->getI1() << "mA";
  actab << "CURRENT_BATT2" << m_ciscrea->getI2() << "mA";
  actab << "VOLTAGE_BATT1" << m_ciscrea->getU1()/1000.0 << "V";
  actab << "VOLTAGE_BATT2" << m_ciscrea->getU2()/1000.0 << "V";
  actab << "CONSUMPTION_BATT1" << m_ciscrea->getCapBat1() << "W";
  actab << "CONSUMPTION_BATT2" << m_ciscrea->getCapBat2() << "W";
  actab << "HEADING" << m_ciscrea->getDirec()/10.0 << "Degrees";
  m_msgs << actab.getFormattedString();

  return(true);
}

bool CISCREA::connectToAUV()
{
  m_ciscrea = new M6dbus(m_serial_port);
  m_ciscrea->setMaxRetries(m_max_modbus_retries);
  bool on = false;
  for (int i = 0;i < m_max_turn_on_retries;++i)
    if (m_ciscrea->getOn())
    {
      on = true;
      Notify("CISCREA_ON", "true");
      break;
    }
  return on;
}


