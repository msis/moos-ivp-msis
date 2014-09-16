/**
 * \file EchoSounder.h
 * \brief Classe EchoSounder
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS récupérant les données de l'écho-sondeur
 *
 */

#ifndef EchoSounder_HEADER
#define EchoSounder_HEADER

#include "MOOS/libMOOS/App/MOOSApp.h"
#ifdef _WIN32
	#include "MOOS/libMOOS/Utils/MOOSNTSerialPort.h"
#else
	#include "MOOS/libMOOS/Utils/MOOSLinuxSerialPort.h"
#endif

#define NOMBRE_CARACTERES_REPONSE_ECHOSONDEUR	80

using namespace std;

class EchoSounder : public CMOOSApp
{
	public:
		EchoSounder();
		bool initialiserPortSerie(string nom_port);
		double getDistancePremierObstacle();
		~EchoSounder();

	protected:
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();

	private: // Configuration variables

	private: // State variables
		unsigned int			m_iterations;
		double					m_timewarp;
		CMOOSLinuxSerialPort	m_moos_serial_port;
		bool					m_port_initialise;
};

#endif 
