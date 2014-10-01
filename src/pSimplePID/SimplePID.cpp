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

  double d_goal = GetMOOSVar("GOAL")->GetDoubleVal();
  double d_current = GetMOOSVar("CURRENT")->GetDoubleVal();
  double d_output = 0;

  double d_error = d_current - d_goal;

  m_pid.Run(d_error,MOOSTime(),d_output);

  // enforce output
  MOOSAbsLimit(d_output,m_max_output);

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
  string s_goal,s_current,s_output;

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
    }
  }

  m_pid.SetGains(d_kp,d_kd,d_ki);
  m_pid.SetLimits(d_ilimit,100);

  double period = 0.05;
  AddMOOSVariable("GOAL",s_goal,"",period);
  AddMOOSVariable("CURRENT",s_current,"",period);
  AddMOOSVariable("OUTPUT","",s_output,period);
  
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

