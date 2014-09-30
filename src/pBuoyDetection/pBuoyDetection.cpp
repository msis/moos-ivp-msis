/**
 * \file pBuoyDetection.cpp
 * \brief Classe pBuoyDetection
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 */

#include "pBuoyDetection.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */

pBuoyDetection::pBuoyDetection() : 
     show_process(true),
     hasImage(false),
     message_name("Buoy"),
     path_save(""),
     folder_name_pattern("/DATASET_%F_%H-%M-%S"),
     image_name_pattern("/image_%H_%M_%S.jpg")
{
    m_iterations = 0;
    m_timewarp   = 1;
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

pBuoyDetection::~pBuoyDetection()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */

bool pBuoyDetection::OnNewMail(MOOSMSG_LIST &NewMail)
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
            memcpy(img.data, msg.GetBinaryData(), img.rows*img.step);
            hasImage = true;
        }
    }

    return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */

bool pBuoyDetection::OnConnectToServer()
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

bool pBuoyDetection::Iterate()
{
    m_iterations++;
//    img = imread("/home/schvarcz/Desktop/Missão Eurathlon/9th/PICS_2014-09-24_16-30-00/BOTTOM_16_31_43.jpg");
//    hasImage = true;
    if (hasImage)
        detect(img);
    return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */

bool pBuoyDetection::OnStartUp()
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
            if(param == "VARIABLE_WHEN_FOUND")
            {
                message_name = value;
            }
            if(param == "SHOW_PROCESS")
            {
                show_process = (value == "true");
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

void pBuoyDetection::RegisterVariables()
{
    // m_Comms.Register("FOOBAR", 0);
    m_Comms.Register(image_name, 0);
}

void pBuoyDetection::detect(Mat img)
{
    Mat imgHSV,imgThr,imgThr2,eqHSV;
    img = img(Rect(10,10,img.cols-20,img.rows-20));
    blur(img,img,Size(3,3));
    blur(img,img,Size(3,3));
    cvtColor(img,imgHSV,CV_RGB2HSV);
    vector<Mat> channels;
    split(imgHSV,channels);
    for (int i =0; i<channels.size();i++)
        equalizeHist(channels[i],channels[i]);
    merge(channels,eqHSV);

    inRange(imgHSV,Scalar(95,100,0),Scalar(200,200,255),imgThr);
    inRange(eqHSV,Scalar(250,240,250),Scalar(255,255,255),imgThr2);

    Moments m1 = moments(imgThr);
    Point2f centerDetection(0,0);

    int found = 0;
    if (m1.m00 != 0)
    {
        centerDetection.x = m1.m10/m1.m00;
        centerDetection.y = m1.m01/m1.m00;
        found += 1;
        if (show_process)
        {
            Point center1(m1.m10/m1.m00,m1.m01/m1.m00);
            circle(img,center1,5,Scalar(0,0,255));
        }
    }
    Moments m2 = moments(imgThr2);
    if (m2.m00 != 0)
    {
        centerDetection.x += m2.m10/m2.m00;
        centerDetection.y += m2.m01/m2.m00;
        if (found == 1)
        {
            centerDetection.x /= 2.;
            centerDetection.y /= 2.;
        }
        found += 2;
        if (show_process)
        {
            Point center2(m2.m10/m2.m00,m2.m01/m2.m00);
            circle(img,center2,5,Scalar(255,0,0));
        }
    }

    centerDetection.x -= img.cols/2.;
    centerDetection.y -= img.rows/2.;

    if (found > 0)
    {
        char name[80];
        time_t     now = time(0);
        struct tm  tstruct;
        tstruct = *localtime(&now);
        // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
        // for more information about date/time format
        strftime(name, sizeof(name), image_name_pattern.c_str(), &tstruct);
        imwrite(path_save+name, img);

        char message[80];
        sprintf(message,"x=%f,y=%f,img_w=%d,img_h=%d",centerDetection.x,-centerDetection.y,img.cols,img.rows);

        m_Comms.Notify(message_name, message);
    }
    else
        m_Comms.Notify(message_name, "false");

    if(show_process)
    {
        imshow("Original",img);
        imshow("Result",imgThr);
        imshow("Result",imgThr);
        imshow("Result2",imgThr2);
        imshow("imghsv",imgHSV);
        imshow("Heq",eqHSV);
        waitKey(20);
    }
}
