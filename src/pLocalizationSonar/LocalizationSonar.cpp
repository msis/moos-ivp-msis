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

LocalizationSonar::LocalizationSonar(): 
    hasImage(false),
    showProcess(true),
    use_razor_heading(false),
    requestLocalization(false),
    pool(50.,50.),
    pool_utm(0.,0.) //width, height
{
    m_iterations = 0;
    m_timewarp   = 1;
    timeWindow   = 50;
    startAngle   = 5000;
    pool_angle   = MOOSDeg2Rad(0.0);
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

        if( msg.GetKey() == "YAW" && use_razor_heading)
        {
            heading_razor = MOOSDeg2Rad(msg.GetDouble());
            double a = MOOSDeg2Rad(-12.6), b = 0.45, c = MOOSDeg2Rad(-10.5);
            heading = heading_razor - ( a*sin(heading_razor+b) + c);

            //heading += pool_angle;
        }
        if( msg.GetKey() == "HEADING" && !use_razor_heading)
        {
            heading = MOOSDeg2Rad(msg.GetDouble());
            //heading += pool_angle;
        }
        if( msg.GetKey() == "REQUEST_FOR_LOCALIZATION")
        {
            if(showProcess)
                cout << endl << endl << "Request for localization" << endl;
            requestLocalization = true;
        }
        if( msg.GetKey() == "SONAR_RAW_DATA" && requestLocalization)
        {
            float angle = 0;
            MOOSValFromString(angle, msg.GetString(), "bearing");
            if(showProcess)
                cout << "Bearing: " << angle << " - " << abs(startAngle - angle) << endl;
            readings++;
            
            if (startAngle == 5000)
            {
                startAngle = angle;
                readings = 0;
            }
            
            if((abs(startAngle - angle) < 3) && (readings>40))
            {
                hasImage = true;
            }
            vector<unsigned int> scanline;
            int nRows, nCols;
            MOOSValFromString(scanline, nRows, nCols, msg.GetString(), "scanline");

            float ad_interval = 0.25056;
            MOOSValFromString(ad_interval, msg.GetString(), "ad_interval");
            double mag_step = sonarScale * ad_interval / 2.0;

            for (double alpha = angle-2.; alpha <angle+2.; alpha+=0.5)
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
//            float alpha = 0.99;
//            addWeighted( sonarImg, alpha, Mat::zeros(Size(sonarImg.cols,sonarImg.rows),sonarImg.type()), 1.-alpha, 0.0, sonarImg);
//            blur(sonarImg,sonarImg,Size(3,3));
            //threshold(sonarImg,sonarImg,60,255,CV_THRESH_TOZERO);
            if(showProcess)
            {
                imshow("Original", sonarImg);
                waitKey(33);
            }
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
    if (hasImage)
    {
        processImage(sonarImg);
    }
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
                pool_angle = atof((char*)value.c_str());

            if(param == "TIME_WINDOW")
                timeWindow = atoi((char*)value.c_str());
                
            if(param == "SONAR_SCALE")
                sonarScale = atof((char*)value.c_str());
                
            if(param == "USE_RAZOR_HEADING")
                use_razor_heading = (value == "true");
                
            if(param == "SHOW_PROCESS")
                showProcess = (value == "true");
        }
    }

    ptTime = new int**[timeWindow];
    for(unsigned int i=0;i<timeWindow;i++)
    {
        ptTime[i] = new int*[4];
        for(unsigned int j=0;j<4;j++)
        {
            ptTime[i][j] = new int[2];
            ptTime[i][j][0] = 0;
            ptTime[i][j][1] = 0;
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

void LocalizationSonar::RegisterVariables()
{
    // m_Comms.Register("FOOBAR", 0);
    m_Comms.Register("SONAR_RAW_DATA", 0);
    m_Comms.Register("YAW", 0);
    m_Comms.Register("HEADING", 0);
    m_Comms.Register("REQUEST_FOR_LOCALIZATION", 0);
}

void LocalizationSonar::processImage(Mat img)
{
    found = false;
    img.copyTo(gray);
    char name[80];
    //sprintf(name,"/home/schvarcz/Desktop/sonar/SONAR_%06d.jpg",m_iterations);
    //imwrite(name,gray);
    Mat er,er2,blur,eq,thresh, sharpe, poly = Mat::zeros(Size(400,400),img.type());
    
    //imshow("Original",gray);
    GaussianBlur(gray,blur,Size(11,11),0);
    if (showProcess)
        imshow("Blur",blur);
    //GaussianBlur(gray,blur,Size(7,7),0);
    threshold(blur,blur,15,255,CV_THRESH_TOZERO);
    
    if (showProcess)
        imshow("Blur2",blur);
    GaussianBlur(blur,sharpe,Size(9,9),0);
    addWeighted(blur,1.5,sharpe,-0.5,0,sharpe);
    
    if (showProcess)
        imshow("Sharpe",sharpe);
    equalizeHist(sharpe,eq);
    
    if (showProcess)
        imshow("equalizeHist",eq);
    threshold(eq,thresh,205,255,CV_THRESH_BINARY);
    
    if (showProcess)
        imshow("threshold",thresh);
    
    erode(thresh,thresh,Mat(Size(3,3),CV_8U),Point(-1,-1),4);
    
    if (showProcess)
        imshow("Erode",thresh);
    vector<vector<Point> > contours, contoursToDraw;
    findContours(thresh,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    for (int i =0;i<contours.size();i++)
    {
        if (contourArea(contours.at(i)) >200)
        {
            contoursToDraw.push_back(contours.at(i));
        }
    }
    
    drawContours(poly,contoursToDraw,-1,Scalar(255,255,255),-1);
    if (showProcess)
    {
        imshow("poly",poly);
    }
    
//    erode(poly,er,Mat(Size(3,3),CV_8U),Point(-1,-1),5);
//    imshow("Erode",er);
    if (robot.x <10 || abs(pool.width-robot.x) <10 || robot.y <10)
        dilate(poly,er2,Mat(Size(3,3),CV_8U),Point(-1,-1),11);
    else
        dilate(poly,er2,Mat(Size(3,3),CV_8U),Point(-1,-1),9);
    
    if (showProcess)
        imshow("Dilate",er2);
    erode(er2,er,Mat(Size(3,3),CV_8U),Point(-1,-1),4);
    
    if (showProcess)
        imshow("Erode2",er);
    //dilate(er,er2,Mat(Size(3,3),CV_8U),Point(-1,-1),15);
    
    if (showProcess)
        imshow("Dilate2",er2);

    Canny(er,edge,90,150);
    
    if (showProcess)
        imshow("edges",edge);
        
    if (showProcess)
        cvtColor( gray, color_dst, CV_GRAY2BGR );

    computePoints(edge);
    updateAverageTimeWindow();

    
    if (showProcess)
        drawPoints();

    if (found)
    {
        m_iterations++;
        //My "sorting"... please! don't blame me!
        int min1=-1, min2=-1;
        for(int i =0; i<4;i++)
        {
            if (min1 == -1)
            {
                min1=i;
            }
            else if(min2 ==-1)
            {
                min2=i;
            }
            else if(pts[min1].y > pts[i].y)
            {
                if(pts[min2].y > pts[min1].y)
                    min2 = min1;
                min1 = i;
            }
            else if(pts[min2].y > pts[i].y)
            {
                min2 = i;
            }
        }
        if(pts[min1].x > pts[min2].x)
        {
            int aux = min1;
            min1 = min2;
            min2 = aux;
        }
        topLeft = pts[min1];
        topRight = pts[min2];
        
        if(showProcess)
        {
            circle(color_dst,topLeft,3,Scalar(255,100,150),-1);
            circle(color_dst,topRight,3,Scalar(255,255,0),-1);
            imshow("Result",color_dst);
        }
        
        float distance = sqrt(pow(topLeft.x-topRight.x,2) + pow(topLeft.y-topRight.y,2));
        float resolution = pool.width/distance;
        Point2f origin = translatePt2f(topRight,Point2f(-img.cols/2.,-img.rows/2.));
        origin = rotatePt2f(origin,MOOSDeg2Rad(lastPoolDetected.angle));
        //origin = translatePt2f(origin,Point2f(img.cols/2.,img.rows/2.));
        robot.x = -origin.x*resolution;
        robot.y = origin.y*resolution;
        
        robot = rotatePt2f(robot,-pool_angle);

        //printf("x = %f\t y = %f meters\n",robot.x, robot.y);

        double angle = pool_angle;
        robotUTM.x = robot.x*cos(angle) + robot.y*sin(angle) + pool_utm.x;
        robotUTM.y = -robot.x*sin(angle) + robot.y*cos(angle) + pool_utm.y;
        
        m_Comms.Notify("NAV_X",robot.x);
        m_Comms.Notify("NAV_Y",robot.y);
        m_Comms.Notify("NAV_UTM_X",robotUTM.x);
        m_Comms.Notify("NAV_UTM_Y",robotUTM.y);
        hasImage = false;
        requestLocalization = false;
        startAngle = 5000;
    }

    int k = waitKey(33) & 255;
    if (k == 10)
    {
        cout << "Saved" << endl;
        imwrite("simon.jpg",img);
    }

}

void LocalizationSonar::computePoints(Mat &sonarEdges)
{

    vector<vector<Point> > contours;
    findContours(sonarEdges,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    int max = -1;

    for (int i =0;i<contours.size();i++)
    {
        if ((max<0) || (arcLength(contours.at(max),false) < arcLength(contours.at(i),false)))
        {
            max = i;
        }
    }
    if (max >= 0)
    {
        if(showProcess)
            drawContours(color_dst,contours,max,Scalar(0,0,255),-1);
        RotatedRect rect = minAreaRect(contours.at(max));
        if ((rect.size.width > 200) && (rect.size.height > 200))
            if ((rect.size.width < 275) && (rect.size.height < 275))
            {
                found = true;
                rect.points(pts);
                lastPoolDetected = rect;
            }
    }
}

void LocalizationSonar::updateAverageTimeWindow()
{
    if (m_iterations == 0)
        return;

    for(unsigned int i=0;i<4;i++)
    {
        ptsW[i].x = 0;
        ptsW[i].y = 0;
        ptTime[m_iterations%timeWindow][i][0] = pts[i].x;
        ptTime[m_iterations%timeWindow][i][1] = pts[i].y;
    }

    for(unsigned int i =0;i<m_iterations && i<timeWindow;i++)
    {
        for(unsigned int j =0;j<4;j++)
        {
            ptsW[j].x += ptTime[i][j][0];
            ptsW[j].y += ptTime[i][j][1];
        }
    }
    int divisor = min(m_iterations,timeWindow);
    
    for(unsigned int i=0;i<4;i++)
    {
        ptsW[i].x /= divisor;
        ptsW[i].y /= divisor;
    }
}

void LocalizationSonar::drawPoints()
{
    line(color_dst,pts[0], pts[1], Scalar(255,0,255));
    line(color_dst,pts[0], pts[3], Scalar(255,0,255));
    line(color_dst,pts[1], pts[2], Scalar(255,0,255));
    line(color_dst,pts[2], pts[3], Scalar(255,0,255));

    for(unsigned int i=0;i<4;i++)
        circle(color_dst,pts[i],3,Scalar(255,0,0),-1);
    
    line(color_dst,ptsW[0], ptsW[1], Scalar(150,255,255));
    line(color_dst,ptsW[0], ptsW[3], Scalar(150,255,255));
    line(color_dst,ptsW[1], ptsW[2], Scalar(150,255,255));
    line(color_dst,ptsW[2], ptsW[3], Scalar(150,255,255));

    for(unsigned int i=0;i<4;i++)
        circle(color_dst,ptsW[i],3,Scalar(150,255,255),-1);
}

Point2f LocalizationSonar::rotatePt2f(Point2f pt, float angle)
{
    Point2f ret;
    ret.x = pt.x*cos(angle) + pt.y*sin(angle);
    ret.y = -pt.x*sin(angle) +pt.y*cos(angle);
    return ret;
}

Point2f LocalizationSonar::translatePt2f(Point2f pt, Point2f center)
{
    Point2f ret;
    ret.x = pt.x + center.x;
    ret.y = pt.y + center.y;
    return ret;
}

