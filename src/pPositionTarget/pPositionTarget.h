/**
 * \file pPositionTarget.h
 * \brief Classe pPositionTarget
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 */

#ifndef pPositionTarget_HEADER
#define pPositionTarget_HEADER

#include <iterator>
#include "MBUtils.h"
#include "MOOS/libMOOS/App/MOOSApp.h"
#include "Ciscrea_Images/Ciscrea_Images.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <time.h>
#include <sys/stat.h>

using namespace std;
using namespace cv;

class pPositionTarget : public CMOOSApp
{
    public:
        pPositionTarget();
        ~pPositionTarget();

    protected:
        bool OnNewMail(MOOSMSG_LIST &NewMail);
        bool Iterate();
        bool OnConnectToServer();
        bool OnStartUp();
        void RegisterVariables();

    private: // Configuration variables

    private: // State variables
        unsigned int    m_iterations;
        double            m_timewarp;
        double            m_start_time;
        double nav_x,nav_y;
        struct tm  start;
        string image_name, path_save, image_name_pattern, folder_name_pattern;
        bool show_image, start_record;
        Mat img;
};

#endif 
