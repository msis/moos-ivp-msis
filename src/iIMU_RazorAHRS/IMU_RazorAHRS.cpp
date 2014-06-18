/************************************************************/
/*    NAME:                                               */
/*    ORGN: MIT                                             */
/*    FILE: IMU_RazorAHRS.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "IMU_RazorAHRS.h"


//---------------------------------------------------------
// Constructor

IMU_RazorAHRS::IMU_RazorAHRS()
{
  m_timewarp   = 1;

  m_gyro_x = 0;
  m_gyro_y = 0;
  m_gyro_z = 0;
  m_mag_x = 0;
  m_mag_y = 0;
  m_mag_z = 0;
  m_acc_x = 0;
  m_acc_y = 0;
  m_acc_z = 0;
  m_roll = 0;
  m_pitch = 0;
  m_yaw = 0;

  m_port_name = "";
}

//---------------------------------------------------------
// Destructor

IMU_RazorAHRS::~IMU_RazorAHRS()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool IMU_RazorAHRS::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
   
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool IMU_RazorAHRS::OnConnectToServer()
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

bool IMU_RazorAHRS::Iterate()
{
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool IMU_RazorAHRS::OnStartUp()
{
  setlocale(LC_ALL,"C");

  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);
      
      if(param == "SERIAL_PORT") {
        m_port_name = value;
      }
      else if(param == "MODE") {
        if (value == "RAW_SENSOR_DATA") {
          m_razor_mode = RazorAHRS::ACC_MAG_GYR_RAW;
        }
        else if (value == "CALIBRATED_SENSOR_DATA") {
          m_razor_mode = RazorAHRS::ACC_MAG_GYR_CALIBRATED;
        }
        else if (value == "ANGLES_CALIBRATED_SENSOR_DATA") {
          m_razor_mode = RazorAHRS::YAW_PITCH_ROLL_ACC_MAG_GYR_CALIBRATED;
        }
        else if (value == "ANGLES_RAW_SENSOR_DATA") {
          m_razor_mode = RazorAHRS::YAW_PITCH_ROLL_ACC_MAG_GYR_RAW;
        }
        else if (value == "ANGLES") {
          m_razor_mode = RazorAHRS::YAW_PITCH_ROLL;
        }
      }
    }
  }
  if (!InitRazorAHRS())
    return(false);
  
  m_timewarp = GetMOOSTimeWarp();

  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void IMU_RazorAHRS::RegisterVariables()
{
  // m_Comms.Register("FOOBAR", 0);
}

bool IMU_RazorAHRS::InitRazorAHRS()
{
  bool init = true;
  try {
    m_razor = new RazorAHRS(m_port_name,
                  bind(&IMU_RazorAHRS::OnRazorData, this, _1),
                  bind(&IMU_RazorAHRS::OnRazorError, this, _1),
                  m_razor_mode);
  }
  catch (runtime_error &e)
  {
    cout << "  " << (string("Could not create tracker: ") + string(e.what())) << endl;
    cout << "  " << "Did you set the serial port in the configuration file?" << endl;
    cout << "  " << "Also make sure that you have R/W access to that port." << endl;
    init = false;
  }
  return init;
}

void IMU_RazorAHRS::OnRazorError(const string &msg)
{
  cout << "  " << "ERROR: " << msg << endl;
}

void IMU_RazorAHRS::OnRazorData(const float data[])
{
  if (m_razor_mode==RazorAHRS::YAW_PITCH_ROLL)
  {
    m_yaw = data[0];
    m_pitch = data[1];
    m_roll = data[2];
    m_Comms.Notify("YAW",m_yaw);
    m_Comms.Notify("PITCH",m_pitch);
    m_Comms.Notify("ROLL",m_roll);
  } 
  else if ((m_razor_mode==RazorAHRS::YAW_PITCH_ROLL_ACC_MAG_GYR_RAW)
    ||(m_razor_mode==RazorAHRS::YAW_PITCH_ROLL_ACC_MAG_GYR_CALIBRATED))
  {
    m_yaw = data[0];
    m_pitch = data[1];
    m_roll = data[2];
    m_acc_x = data[3];
    m_acc_y = data[4];
    m_acc_z = data[5];
    m_mag_x = data[6];
    m_mag_y = data[7];
    m_mag_z = data[8];
    m_gyro_x = data[9];
    m_gyro_y = data[10];
    m_gyro_z = data[11];
    m_Comms.Notify("YAW",m_yaw);
    m_Comms.Notify("PITCH",m_pitch);
    m_Comms.Notify("ROLL",m_roll);
    m_Comms.Notify("ACC_X",m_acc_x);
    m_Comms.Notify("ACC_Y",m_acc_y);
    m_Comms.Notify("ACC_Z",m_acc_z);
    m_Comms.Notify("MAG_X",m_mag_x);
    m_Comms.Notify("MAG_Y",m_mag_y);
    m_Comms.Notify("MAG_Z",m_mag_z);
    m_Comms.Notify("GYRO_X",m_gyro_x);
    m_Comms.Notify("GYRO_Y",m_gyro_y);
    m_Comms.Notify("GYRO_Z",m_gyro_z);
  }
  else if ((m_razor_mode==RazorAHRS::ACC_MAG_GYR_CALIBRATED)
    ||(m_razor_mode==RazorAHRS::ACC_MAG_GYR_RAW))
  {
    m_acc_x = data[0];
    m_acc_y = data[1];
    m_acc_z = data[2];
    m_mag_x = data[3];
    m_mag_y = data[4];
    m_mag_z = data[5];
    m_gyro_x = data[6];
    m_gyro_y = data[7];
    m_gyro_z = data[8];
    m_Comms.Notify("ACC_X",m_acc_x);
    m_Comms.Notify("ACC_Y",m_acc_y);
    m_Comms.Notify("ACC_Z",m_acc_z);
    m_Comms.Notify("MAG_X",m_mag_x);
    m_Comms.Notify("MAG_Y",m_mag_y);
    m_Comms.Notify("MAG_Z",m_mag_z);
    m_Comms.Notify("GYRO_X",m_gyro_x);
    m_Comms.Notify("GYRO_Y",m_gyro_y);
    m_Comms.Notify("GYRO_Z",m_gyro_z);
  }
}