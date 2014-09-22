/**
 * \file Joystick.cpp
 * \brief Classe Joystick
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 7th 2013
 *
 * Application MOOS donnant les consignes de déplacement de l'AUV
 *
 */

#include <iterator>
#include "MBUtils.h"
#include "Joystick.h"

//#include <fcntl.h>
#include <sys/fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
//#include <sys/types.h>

#include <unistd.h>

#include <linux/joystick.h>

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
Joystick::Joystick()
{
  device_name = "/dev/input/js0";
  trace_all_events = false;
	m_iterations = 0;
	m_timewarp   = 1;
	
	m_deviceReadThread.Initialise(read_loop_thread_function, (void*)this);
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

Joystick::~Joystick()
{
  m_deviceReadThread.Stop();
  close(fd);
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool Joystick::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin() ; p != NewMail.end() ; p++)
	{
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

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool Joystick::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", 0);

	RegisterVariables();
	

	
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée automatiquement périodiquement
 * Implémentation du comportement de l'application
 */
 
bool Joystick::Iterate()
{
	m_iterations++;

	map<string, double>::const_iterator it;
	// Update incremental variables
	for (it=increment_variable.begin(); it!=increment_variable.end(); ++it) {
		variables[it->first] += it->second;
	}
	
	// Refresh variables	
	for (it=variables.begin(); it!=variables.end(); ++it) {
		Notify(it->first, it->second);
	}
	
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool Joystick::OnStartUp()
{
  double ratio = 1.0;
  setlocale(LC_ALL, "C");
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams))
  {
    list<string>::reverse_iterator p;

    int axis_id = -1;
    int button_id = -1;
    
    double axis_min = -1.0;
    double axis_max = 1.0;
    bool increment = false;

    for(p = sParams.rbegin() ; p != sParams.rend() ; p++)
    {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);

      MOOSTrace("%s=%s\n", param.c_str(), value.c_str());
      
      if(param == "DEVICE_NAME")
      {
	device_name = value;
	//handled
      }
      else if(param == "INCREMENT")
      {
	increment = atof(value.c_str());
      }
      else if(param == "SHOW_ALL_EVENTS")
      {
	trace_all_events = (atoi(value.c_str()) != 0);
      }
      else if(param == "SCALE")
      {
	axis_max = -atof(value.c_str());
	axis_min = -axis_max;
      }
      else if(param == "MIN")
      {
	axis_min = atof(value.c_str());
      }
      else if(param == "MAX")
      {
	axis_max = atof(value.c_str());
      }
      else if(param == "AXIS_SCALE_RATIO")
      {
	ratio = atof(value.c_str());
	ratio = atof(value.c_str());
      }
      else if(param == "AXIS")
      {
	axis_id = atoi(value.c_str());
	button_id = -1;
      }
      else if(param == "BUTTON")
      {
	button_id = atoi(value.c_str());
	axis_id = -1;
      }
      else if(param == "DEST_VAR")
      {
	if (button_id > 0)
	{
	  button_variables[button_id] = value;
	  axis_gain[make_pair(JS_EVENT_BUTTON,button_id)]   = ratio*axis_max - ratio*axis_min;
	  axis_offset[make_pair(JS_EVENT_BUTTON,button_id)] = ratio*axis_min;
	}
	else
// 0->min 1->max f(x)=(max-min)x + min
// -32768->0 32767->1 : g(x)=x/65536 + 0.5
// f(g(x)) = (max-min)(x/65k + 0.5) + min
//         = x(m-m)/65k +0.5(max+min)
	{
	  axis_variables[axis_id] = value;
	  axis_gain[make_pair(JS_EVENT_AXIS,axis_id)]   = (ratio*axis_max - ratio*axis_min) / 65536.;
	  axis_offset[make_pair(JS_EVENT_AXIS,axis_id)] = (ratio*axis_max + ratio*axis_min) * 0.5;
	}
	
	if (increment)
	  increment_variable[value] = 0.0;
      }
    }
  }

  m_timewarp = GetMOOSTimeWarp();

  fd = open(device_name.c_str(), O_RDONLY);
  if (fd == -1) MOOSFail("Cannot open the joystick device '%s'.\n", device_name.c_str());
  
  m_deviceReadThread.Start();

  RegisterVariables();
  return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void Joystick::RegisterVariables()
{
	// m_Comms.Register("FOOBAR", 0);
}

bool Joystick::deviceReadLoop()
{
  while (!m_deviceReadThread.IsQuitRequested())
  {
    js_event e;
    fd_set fdset;
    timeval timeout = {0, 100000}; // sec, usec
    FD_ZERO(&fdset);
    FD_SET(fd, &fdset);
    
    if (select(fd+1, &fdset, 0, 0, &timeout) > 0)
    {
      read(fd, &e, sizeof(js_event));
      // do not differentiate between synthetic int events and real events
      e.type &= ~JS_EVENT_INIT;
      
      map<int,string>::iterator it;
      double value = axis_gain[make_pair(e.type,e.number)] * e.value 
                     + axis_offset[make_pair(e.type,e.number)];
      
      switch (e.type) {
	case JS_EVENT_BUTTON:
	  if (trace_all_events)
	    MOOSTrace("Button %d=%d\n", e.number, e.value);
	  it = button_variables.find(e.number);
	  if (it != button_variables.end())
	  {
	    if (increment_variable.find(it->second) != increment_variable.end()) {
	      increment_variable[it->second] = value;
	    }
	    else {
	      variables[it->second] = value;
	      Notify(it->second, value);
//	    MOOSTrace("  -> %s\n", it->second.c_str());
	    }
	  }
	  break;
	case JS_EVENT_AXIS:
	  if (trace_all_events)
	    MOOSTrace("Axis %d=%d\n", e.number, e.value);
	  it = axis_variables.find(e.number);
	  if (it != axis_variables.end())
	  {
	      if (increment_variable.find(it->second) != increment_variable.end()) {
	      increment_variable[it->second] = value;
	    }
	    else {
		variables[it->second] = value;
	        Notify(it->second, value);
//	    MOOSTrace("  -> %s\n", it->second.c_str());
	    }
	  }

	  break;
	default:
	  break;
      }
    }
  }
}


bool Joystick::read_loop_thread_function ( void* ptr_this )
{
  return ((Joystick*)ptr_this)->deviceReadLoop();
}
