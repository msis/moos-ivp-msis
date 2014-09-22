/**
 * \file pCameraLogger.h
 * \brief Classe pCameraLogger
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 */

#ifndef pCameraLogger_HEADER
#define pCameraLogger_HEADER

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

class pCameraLogger : public CMOOSApp
{
    public:
        pCameraLogger();
        ~pCameraLogger();

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
        string image_name, path_save, image_name_pattern, folder_name_pattern;
        bool show_image, start_record;
        Mat img;
};

#endif 
