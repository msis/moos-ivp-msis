/************************************************************/
/*    NAME: Mohamed Saad IBN SEDDIK                         */
/*    ORGN: MIT                                             */
/*    FILE: SimplePID.cpp                                   */
/*    DATE:                                                 */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "SimplePID.h"

using namespace std;

//---------------------------------------------------------
// Constructor

SimplePID::SimplePID()
{
  m_iterations = 0;
  m_timewarp   = 1;
  m_active = true;
  m_angular = true;
}

//---------------------------------------------------------
// Destructor

SimplePID::~SimplePID()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool SimplePID::OnNewMail(MOOSMSG_LIST &NewMail)
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
	
//    return(true);

  return UpdateMOOSVariables(NewMail);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SimplePID::OnConnectToServer()
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

bool SimplePID::Iterate()
{
  m_iterations++;
  bool b_last_activation = m_active;

  string s_activation = GetMOOSVar("ACTIVATE")->GetStringVal();
  if (s_activation == "true")
    m_active = true;
  else {
    m_active = false;
  }

  double d_output = 0;
  if (m_active)
  {
    double d_goal = GetMOOSVar("GOAL")->GetDoubleVal();
    double d_current = GetMOOSVar("CURRENT")->GetDoubleVal();
    if (m_angular)
      d_goal = angle180(d_goal);

    double d_error = d_current - d_goal;
    
    if (m_angular)
      d_error = angle180(d_error);

    m_pid.Run(d_error,MOOSTime(),d_output);

    // enforce output
    MOOSAbsLimit(d_output,m_max_output);
    SetMOOSVar("OUTPUT",d_output,MOOSTime());
  }
  if( (b_last_activation!=m_active) && !(m_active))
    SetMOOSVar("OUTPUT",d_output,MOOSTime());
  PublishFreshMOOSVariables();  
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool SimplePID::OnStartUp()
{
  double d_kp,d_ki,d_kd,d_ilimit;
  string s_goal,s_current,s_output,s_activation;

  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);
      double dval = atof(value.c_str());
      
      if(param == "GOAL_NAME") {
        s_goal = value;
      }
      else if(param == "VAR_NAME") {
        s_current = value;
      }
      else if(param == "OUTPUT_NAME") {
        s_output = value;
      }
      else if(param == "MAX_OUTPUT") {
        m_max_output = dval;
      }
      else if(param == "PID_KP") {
        d_kp = dval;
      }
      else if(param == "PID_KD") {
        d_kd = dval;
      }
      else if(param == "PID_KI") {
        d_ki = dval;
      }
      else if(param == "INTEGRAL_LIMIT") {
        d_ilimit = dval;
      }
      else if(param == "ACTIVE_START") {
        value = toupper(value);
        if (value == "FALSE")
        {
          m_active = false;
        } else {
          m_active = true;
        }
      }
      else if(param == "ACTIVATION_VAR") {
        s_activation = value;
      }
      else if(param == "ANGULAR_VAR") {
        if (value == "FALSE")
        {
          m_angular = false;
        } else {
          m_angular = true;
        }
      }
    }
  }

  m_pid.SetGains(d_kp,d_kd,d_ki);
  m_pid.SetLimits(d_ilimit,100);

  double period = 0.05;
  AddMOOSVariable("GOAL",s_goal,"",period);
  AddMOOSVariable("CURRENT",s_current,"",period);
  AddMOOSVariable("OUTPUT","",s_output,period);
  AddMOOSVariable("ACTIVATE",s_activation,"",period);
  
  m_timewarp = GetMOOSTimeWarp();

  RegisterMOOSVariables();
  RegisterVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void SimplePID::RegisterVariables()
{
  // m_Comms.Register("FOOBAR", 0);
}

