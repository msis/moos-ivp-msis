/**
 * \file Joystick.h
 * \brief Classe Joystick
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 7th 2013
 *
 * Application MOOS donnant les consignes de déplacement de l'AUV
 *
 */

#ifndef Joystick_HEADER
#define Joystick_HEADER

#include "MOOS/libMOOS/App/MOOSApp.h"

#include <string>
#include <map>

using namespace std;

class Joystick : public CMOOSApp
{
	public:
		Joystick();
		~Joystick();

	protected:
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();
		
		bool deviceReadLoop();

	private: // Configuration variables
		string device_name;
		map<int, string> axis_variables;
		map<pair<int,int>, double> axis_offset;
		map<pair<int,int>, double> axis_gain;
		map<int, string> button_variables;
		map<string, double> variables;
		map<string, double> increment_variable;
		bool trace_all_events;

	private: // State variables
		unsigned int	m_iterations;
		double		m_timewarp;
		
		int fd;
		CMOOSThread m_deviceReadThread;

	static bool read_loop_thread_function(void *ptr_this);
};

#endif 
