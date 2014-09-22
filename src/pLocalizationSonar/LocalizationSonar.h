/**
 * \file LocalizationSonar.h
 * \brief Classe LocalizationSonar
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 */

#ifndef LocalizationSonar_HEADER
#define LocalizationSonar_HEADER

#include <iterator>
#include "MBUtils.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "MOOS/libMOOS/App/MOOSApp.h"

using namespace std;
using namespace cv;

class LocalizationSonar : public CMOOSApp
{
    public:
        LocalizationSonar();
        ~LocalizationSonar();

    protected:
        bool OnNewMail(MOOSMSG_LIST &NewMail);
        bool Iterate();
        bool OnConnectToServer();
        bool OnStartUp();
        void RegisterVariables();

    private: // Configuration variables
        Point pt1, pt2, pool_utm;
        Point robot, robotUTM;
        Size pool;
        double pool_angle;
        double heading;
        double heading_razor;
        Mat img, edge, gray, color_dst, sonarImg;
        bool hasImage;

        unsigned int timeWindow; //200
        int **ptTime;
        Point ptW1, ptW2;
        void processImage(Mat img);
        void computePoints(Mat &sonarEdges);
        void updateAverageTimeWindow();

    private: // State variables
        unsigned int    m_iterations;
        double            m_timewarp;
};

#endif 
