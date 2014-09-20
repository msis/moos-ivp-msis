/**
 * \file iControl.h
 * \brief Classe iControl
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 */

#ifndef iControl_HEADER
#define iControl_HEADER

#include <iterator>
#include "MBUtils.h"
#include "MOOS/libMOOS/App/MOOSApp.h"
#include "iheadingcontroller.h"

using namespace std;

class iControl : public CMOOSApp
{
    public:
        iControl();
        ~iControl();

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
        double razor_heading,desired_heading;
        iHeadingController ihc;
};

#endif 
