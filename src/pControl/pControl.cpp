/**
 * \file pControl.cpp
 * \brief Classe pControl
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 */

#include "pControl.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */

pControl::pControl() : razor_heading(0.0), desired_heading(0.0)
{
    m_iterations = 0;
    m_timewarp   = 1;
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

pControl::~pControl()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */

bool pControl::OnNewMail(MOOSMSG_LIST &NewMail)
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

        if( msg.GetKey() == "YAW") // HEADING, MAG_Z
        {
            razor_heading = MOOSDeg2Rad(msg.GetDouble());
        }
        if( msg.GetKey() == "DESIRED_HEADING")
        {
            desired_heading = MOOSDeg2Rad(msg.GetDouble());
        }
    }

    return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */

bool pControl::OnConnectToServer()
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

bool pControl::Iterate()
{
    double motor_force = ihc.heading(razor_heading,desired_heading);
    m_iterations++;
    cout << "YAW: " << razor_heading << endl
         << "DESIRED: " << desired_heading << endl
         << "DESIRED_RUDDER: " << motor_force << endl << endl;
    
    m_Comms.Notify("DESIRED_RUDDER", motor_force);
    return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */

bool pControl::OnStartUp()
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

            if(param == "POOL_WIDTH")
            {
                //pool.width = atof((char*)value.c_str());
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

void pControl::RegisterVariables()
{
    // m_Comms.Register("FOOBAR", 0);
    m_Comms.Register("YAW", 0);
    m_Comms.Register("DESIRED_HEADING", 0);
}

