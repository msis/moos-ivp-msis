/**
 * \file pCameraLogger.cpp
 * \brief Classe pCameraLogger
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 */

#include "pCameraLogger.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */

pCameraLogger::pCameraLogger() : show_image(false),
                                 start_record(false),
                                 path_save(""),
                                 image_name_pattern("/frame_%06d.jpg")
{
    m_iterations = 0;
    m_timewarp   = 1;
	img.create(HAUTEUR_IMAGE_CAMERA,LARGEUR_IMAGE_CAMERA,CV_8UC3);
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

pCameraLogger::~pCameraLogger()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */

bool pCameraLogger::OnNewMail(MOOSMSG_LIST &NewMail)
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

        if( msg.GetKey() == image_name)
        {
            start_record = true;
            memcpy(img.data, msg.GetBinaryData(), img.rows*img.step);
        }
    }

    return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */

bool pCameraLogger::OnConnectToServer()
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

bool pCameraLogger::Iterate()
{
    if (!start_record)
        return true;

    if(show_image)
    {
        imshow("Camera Logger: " + image_name,img);
        waitKey(10);
    }
    
    if (path_save == "")
    {
        cout << "WARNING (pCameraLogger): You don't informed where I should save the images!" << endl;
        return true;
    }
    
    m_iterations++;
    char name[18];
    sprintf(name,image_name_pattern.c_str(),m_iterations);
    imwrite(path_save+name, img);
    m_Comms.Notify("IMAGE_SAVED", path_save+name);
    return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */

bool pCameraLogger::OnStartUp()
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

    RegisterVariables();
    return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */

void pCameraLogger::RegisterVariables()
{
    // m_Comms.Register("FOOBAR", 0);
    m_Comms.Register(image_name, 0);
}

