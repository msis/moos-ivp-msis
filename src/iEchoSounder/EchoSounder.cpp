/**
 * \file EchoSounder.cpp
 * \brief Classe EchoSounder
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS récupérant les données de l'écho-sondeur
 *
 */
 
#include <time.h>
#include <iterator>
#include <string.h>
#include "ColorParse.h"
#include "MBUtils.h"
#include "EchoSounder.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
EchoSounder::EchoSounder()
{
	this->m_iterations = 0;
	this->m_timewarp   = 1;
	this->m_port_initialise = false;
}

/**
 * \fn
 * \brief Méthode initialisant le port série
 */
 
bool EchoSounder::initialiserPortSerie(string nom_port)
{
	ifstream fichier(nom_port.c_str()); 
	if(fichier.fail())
		return false;
    
	// Instanciation de l'objet de communication avec le port série
	cout << "Initialisation de \"" << nom_port << "\"" << endl;
	
	this->m_moos_serial_port = CMOOSLinuxSerialPort();
	this->m_port_initialise = this->m_moos_serial_port.Create((char*)nom_port.c_str());
	return this->m_port_initialise;
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

EchoSounder::~EchoSounder()
{
	if(this->m_port_initialise)
		this->m_moos_serial_port.Close();
}

/**
 * \fn
 * \brief Méthode envoyant un message par modem
 */

double EchoSounder::getDistancePremierObstacle()
{
	int resultat_lecture;
	double distance;
	
	if(!this->m_port_initialise)
		return -1;
	
	char reponse_echo[NOMBRE_CARACTERES_REPONSE_ECHOSONDEUR];
	resultat_lecture = this->m_moos_serial_port.ReadNWithTimeOut(reponse_echo, NOMBRE_CARACTERES_REPONSE_ECHOSONDEUR);
	
	// Exemple de retour : "001.567m"
	
	if(resultat_lecture)
	{
		char reponse_parsee[8];
		
		// Enregistrement de la partie de la chaine qui nous intéresse
		for(int i = 0 ; i < 7 ; i ++)
			reponse_parsee[i] = reponse_echo[i];
		reponse_parsee[7] = '\0';
		
		// Conversion en décimales
		distance = atof(reponse_parsee);
		
		if(distance != 0.0)
			return distance;
	}
	
	return -1;
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool EchoSounder::OnNewMail(MOOSMSG_LIST &NewMail)
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
 
bool EchoSounder::OnConnectToServer()
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
 
bool EchoSounder::Iterate()
{
	double distance;
	m_iterations++;
	
	if(!this->m_port_initialise)
		return false;

	distance = getDistancePremierObstacle();
	m_Comms.Notify("ECHOSOUNDER_RAW_DISTANCE", distance);
	m_Comms.Notify("ECHOSOUNDER_DISTANCE", distance);
	
	return true;
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool EchoSounder::OnStartUp()
{
	setlocale(LC_ALL, "C");
	list<string> sParams;
	m_MissionReader.EnableVerbatimQuoting(false);
	if(m_MissionReader.GetConfiguration(GetAppName(), sParams))
	{
		list<string>::iterator p;
		for(p = sParams.begin() ; p != sParams.end() ; p++)
		{
			string original_line = *p;
			string param = stripBlankEnds(toupper(biteString(*p, '=')));
			string value = stripBlankEnds(*p);
			
			if(param == "SERIAL_PORT_NAME")
			{
				if(initialiserPortSerie(value))
					cout << "Port série initialisé sur " << value << endl;
				
				else
					cout << "Échec de l'initialisation du port série sur " << value << endl;
			}
		}
	}

	m_timewarp = GetMOOSTimeWarp();

	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void EchoSounder::RegisterVariables()
{
	//m_Comms.Register(this->m_listeVariablesSuivies[i], 0);
}
