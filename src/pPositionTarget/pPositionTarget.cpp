/**
 * \file pPositionTarget.cpp
 * \brief Classe pPositionTarget
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 */

#include "pPositionTarget.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */

pPositionTarget::pPositionTarget() : show_image(false),
                                 start_record(false),
                                 path_save(""),
                                 folder_name_pattern("/DATASET_%F_%H-%M-%S"),
                                 image_name_pattern("/image_%H_%M_%S.jpg")
{
    m_iterations = 0;
    m_timewarp   = 1;
    
    char name[80];
    time_t now = time(0);
    start = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
	start.tm_year = 2014 -1900;
	start.tm_mon = 9 -1;
	start.tm_mday = 30;
	start.tm_hour = 12;
	start.tm_min = 45;
	start.tm_sec = 14;
    strftime(name, sizeof(name), image_name_pattern.c_str(), &start);
    cout << name << endl;
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

pPositionTarget::~pPositionTarget()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */

bool pPositionTarget::OnNewMail(MOOSMSG_LIST &NewMail)
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

        if( msg.GetKey() == "NAV_X")
        {
            nav_x = msg.GetDouble();
        }
        if( msg.GetKey() == "NAV_Y")
        {
            nav_y = msg.GetDouble();
        }
        if( msg.GetKey() == "PIPE_DETECTION")
        {
            if (msg.GetString() != "false")
            {
                cout << nav_x << ", " << nav_y << endl;
            }
        }
    }

    return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */

bool pPositionTarget::OnConnectToServer()
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

bool pPositionTarget::Iterate()
{
    m_iterations++;
    char name[255];
    
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(name, sizeof(name), "/home/schvarcz/Desktop/Missão Eurathlon/pipe_eurathlon/PICS_%F_12-45-14/SIDE_%H_%M_%S.jpg", &start);
    img = imread(name);
    
	Notify("CAMERA_SIDE", (void*)img.data, 3 * img.rows * img.cols, MOOSLocalTime());

	start.tm_sec += 1;
	if (start.tm_sec >= 60)
	{
	    start.tm_sec =0;
    	start.tm_min += 1;
	}
	if (start.tm_min >= 60)
	{
	    start.tm_min =0;
    	start.tm_hour += 1;
	}
    //m_Comms.Notify(image_name+"_IMAGE_SAVED", path_save+name);
    return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */

bool pPositionTarget::OnStartUp()
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

            if(param == "VARIABLE_IMAGE_NAME")
            {
                image_name = value;
            }
            if(param == "IMAGE_NAME_PATTERN")
            {
                image_name_pattern = "/"+value;
            }
            if(param == "FOLDER_NAME_PATTERN")
            {
                folder_name_pattern = "/"+value;
            }
            if(param == "SAVE_IN_FOLDER")
            {
                path_save = value;
            }
            if(param == "DISPLAY_IMAGE")
            {
                show_image = (value == "true");
            }


        }
    }

    m_timewarp = GetMOOSTimeWarp();
    
    char folder_name[80];
    time_t     now = time(0);
    struct tm  tstruct;
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(folder_name, sizeof(folder_name), folder_name_pattern.c_str(), &tstruct);
    path_save += folder_name;
    mkdir(path_save.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    RegisterVariables();
    return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */

void pPositionTarget::RegisterVariables()
{
    // m_Comms.Register("FOOBAR", 0);
    m_Comms.Register("PIPE_DETECTION", 0);
    m_Comms.Register("NAV_Y", 0);
    m_Comms.Register("NAV_X", 0);
}

