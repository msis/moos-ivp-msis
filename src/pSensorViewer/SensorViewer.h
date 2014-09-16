/**
 * \file SensorViewer.h
 * \brief Classe SensorViewer
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 7th 2013
 *
 * Application MOOS donnant les consignes de déplacement de l'AUV
 *
 */

#ifndef SensorViewer_HEADER
#define SensorViewer_HEADER

#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>


using namespace std;

class SensorViewer : public CMOOSApp
{
	public:
		SensorViewer();
		~SensorViewer();

	protected:
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();

	private: // Configuration variables

	private: // State variables
		unsigned int	m_iterations;
		double			m_timewarp;
		cv::Mat img1, img2;
		cv::Mat img_son_pol;
		cv::Mat img_sonar;
		cv::Mat img_son_cart;
		cv::Mat pol2cart_x;
		cv::Mat pol2cart_y;
		
		double heading;
		double heading_razor;
		double heading_ciscrea;
		
		double vx;
};

#endif 