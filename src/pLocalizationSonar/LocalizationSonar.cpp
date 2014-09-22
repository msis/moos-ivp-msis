/**
 * \file LocalizationSonar.cpp
 * \brief Classe LocalizationSonar
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 */

#include "LocalizationSonar.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */

LocalizationSonar::LocalizationSonar(): hasImage(false), pool(50.,50.), pool_utm(0.,0.) //width, height
{
    m_iterations = 0;
    m_timewarp   = 1;
    timeWindow = 200;
    pool_angle = MOOSDeg2Rad(0.0);
    sonarImg.create(400,400,CV_8UC1);
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

LocalizationSonar::~LocalizationSonar()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */

bool LocalizationSonar::OnNewMail(MOOSMSG_LIST &NewMail)
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

        if( msg.GetKey() == "YAW")
        {
            heading_razor = MOOSDeg2Rad(msg.GetDouble());
            double a = MOOSDeg2Rad(-12.6), b = 0.45, c = MOOSDeg2Rad(-10.5);
            heading = heading_razor - ( a*sin(heading_razor+b) + c);

            heading += pool_angle;
        }
        if( msg.GetKey() == "SONAR_RAW_DATA")
        {
            float angle = 0;
            MOOSValFromString(angle, msg.GetString(), "bearing");
            vector<unsigned int> scanline;
            int nRows, nCols;
            MOOSValFromString(scanline, nRows, nCols, msg.GetString(), "scanline");

            float ad_interval = 0.25056;
            MOOSValFromString(ad_interval, msg.GetString(), "ad_interval");
            //double scale = 60.0;
            double scale = 8.0;
            double mag_step = scale * ad_interval / 2.0;

            for (double alpha = angle-2.0; alpha <angle+2.0; alpha+=0.5)
            {
                double cos_b = cos(MOOSDeg2Rad(-alpha) + heading );
                double sin_b = sin(MOOSDeg2Rad(-alpha) + heading );
                for(unsigned int i=0; i<scanline.size();++i)
                {
                    double d = mag_step * i;
                    int x = sin_b*d + sonarImg.cols/2.-0.5;
                    int y = cos_b*d + sonarImg.rows/2.-0.5;
                    if (x>=0 && x<sonarImg.cols && y>=0 && y<sonarImg.rows)
                        sonarImg.at<unsigned char>(x,y) = scanline[i];
                }
            }
            hasImage = true;
        }
    }

    return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */

bool LocalizationSonar::OnConnectToServer()
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

bool LocalizationSonar::Iterate()
{
    m_iterations++;
    if (hasImage)
        processImage(sonarImg);
    return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */

bool LocalizationSonar::OnStartUp()
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
                pool.width = atof((char*)value.c_str());

            if(param == "POOL_HEIGHT")
                pool.height = atof((char*)value.c_str());

            if(param == "POOL_UTM_X")
                pool_utm.x = atof((char*)value.c_str());

            if(param == "POOL_UTM_Y")
                pool_utm.y = atof((char*)value.c_str());

            if(param == "POOL_ANGLE")
                pool_angle = MOOSDeg2Rad(atof((char*)value.c_str()));

            if(param == "TIME_WINDOW")
                timeWindow = atoi((char*)value.c_str());

        }
    }

    ptTime = new int*[timeWindow];
    for(unsigned int i=0;i<timeWindow;i++)
    {
        ptTime[i] = new int[4];
    }

    m_timewarp = GetMOOSTimeWarp();

    RegisterVariables();
    return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */

void LocalizationSonar::RegisterVariables()
{
    // m_Comms.Register("FOOBAR", 0);
    m_Comms.Register("SONAR_RAW_DATA", 0);
    m_Comms.Register("YAW", 0);
}

void LocalizationSonar::processImage(Mat img)
{
    
    cvtColor(img,gray,CV_RGB2GRAY);
    GaussianBlur(gray,gray,Size(3,3),0);
    Canny(gray,edge,90,150);
    cvtColor( edge, color_dst, CV_GRAY2BGR );

    computePoints(edge);

    updateAverageTimeWindow();


    robot.x = (pt2.x-img.cols/2.0)*pool.width/(pt2.x-pt1.x);
    robot.y = (pt2.y-img.rows/2.0)*pool.height/(pt2.y-pt1.y)    ;

    //printf("x = %d\t y = %d meters\n",robot.x, robot.y);

    double angle = pool_angle;
    robotUTM.x = robot.x*cos(angle) + robot.y*sin(angle);
    robotUTM.y = -robot.x*sin(angle) + robot.y*cos(angle);
    
    m_Comms.Notify("NAV_X",robot.x);
    m_Comms.Notify("NAV_Y",robot.y);
    m_Comms.Notify("NAV_UTM_X",robotUTM.x);
    m_Comms.Notify("NAV_UTM_Y",robotUTM.y);

}

void LocalizationSonar::computePoints(Mat &sonarEdges)
{
    //The main ideia is, how much arrows we have per row/col?
    //Take the first and last row/col if the number of arrows is more than avg.
    int col,row,sumGrayLevel,grayLevel,accumulated=0,average;
    float sumPerColumn[800],sumPerRow[800];
    for(col=0; col<sonarEdges.cols-1; col++)
    {
        sumGrayLevel=0;
        for(row=0; row<sonarEdges.rows-1; row++)
        {
            grayLevel = *(sonarEdges.data + row*sonarEdges.cols + col*sonarEdges.channels() + 1);
            sumGrayLevel = sumGrayLevel+grayLevel;
        }
        sumPerColumn[col] = sumGrayLevel;
        accumulated = accumulated+sumGrayLevel;
    }
    average = accumulated/(sonarEdges.cols-1);

    //Select the first max from left
    for(col=0; col<sonarEdges.cols-1; col++)
    {
        if ((sumPerColumn[col]>average) && (sumPerColumn[col+1] < sumPerColumn[col]))
        {
            pt1.x = col;
            break;
        }
    }

    //Select the first max from right
    for(col=sonarEdges.cols-2; col>2; col--)
    {
        if ((sumPerColumn[col]>average) && (sumPerColumn[col-1] < sumPerColumn[col]))
        {
            pt2.x = col;
            break;
        }
    }

    ///////////---------------------------------
    accumulated=0;average=0;

    for(row=0; row<sonarEdges.rows-1; row++)
    {
        sumGrayLevel=0;
        for(col=0; col<sonarEdges.cols-1; col++)
        {

            grayLevel= *(sonarEdges.data + row*sonarEdges.cols + col*sonarEdges.channels() + 1); //Direct access the memory
            sumGrayLevel=sumGrayLevel+grayLevel;

        }
        sumPerRow[row]=sumGrayLevel;
        accumulated=accumulated+sumGrayLevel;
    }
    average=accumulated/(sonarEdges.rows-1);

    //Select the first max from above
    for(row=0; row<sonarEdges.rows-1; row++)
    {
        if ((sumPerRow[row] > average) && (sumPerRow[row+1] < sumPerRow[row]))
        {
            pt1.y = row;
            break;
        }
    }


    //Select the first max from bottom
    for(row = sonarEdges.rows-2; row>2; row--)
    {
        if ((sumPerRow[row] > average) && (sumPerRow[row-1] < sumPerRow[row]))
        {
            pt2.y = row;
            break;
        }
    }
}

void LocalizationSonar::updateAverageTimeWindow()
{
    ptW1.x = 0;
    ptW1.y = 0;
    ptW2.x = 0;
    ptW2.y = 0;
    ptTime[m_iterations%timeWindow][0] = pt1.x;
    ptTime[m_iterations%timeWindow][1] = pt1.y;
    ptTime[m_iterations%timeWindow][2] = pt2.x;
    ptTime[m_iterations%timeWindow][3] = pt2.y;

    for(unsigned int i =0;i<m_iterations && i<timeWindow;i++)
    {
        ptW1.x += ptTime[i][0];
        ptW1.y += ptTime[i][1];
        ptW2.x += ptTime[i][2];
        ptW2.y += ptTime[i][3];
    }
    int divisor = min(m_iterations,timeWindow);
    ptW1.x /= divisor;
    ptW1.y /= divisor;
    ptW2.x /= divisor;
    ptW2.y /= divisor;
}
