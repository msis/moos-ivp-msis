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
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>
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
        Point pool_utm;
        Point2f pts[4];
        Point ptsW[4];
        Point2f topLeft, topRight;
        Point2f robot, robotUTM;
        Size pool;
        double pool_angle;
        double heading;
        double heading_razor,sonarScale;
        bool found;
        Mat img, edge, gray, color_dst, sonarImg;
        bool hasImage, showProcess, use_razor_heading;
        RotatedRect lastPoolDetected;

        unsigned int timeWindow;
        int ***ptTime;
        void processImage(Mat img);
        void computePoints(Mat &sonarEdges);
        void updateAverageTimeWindow();
        void drawPoints();
        Point2f translatePt2f(Point2f pt, Point2f center);
        Point2f rotatePt2f(Point2f pt, float angle);

    private: // State variables
        unsigned int    m_iterations;
        double            m_timewarp;
};

#endif 
