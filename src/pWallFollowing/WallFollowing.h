/**
 * \file WallFollowing.h
 * \brief Classe WallFollowing
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 25th 2013
 *
 * Application MOOS de suivi de mur
 *
 */

#ifndef WallFollowing_HEADER
#define WallFollowing_HEADER

#include <map>
#include <list>
#include <utility>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "MOOS/libMOOS/App/MOOSApp.h"
#include "AngleUtils.h"

#define LARGEUR_MAPPING		400
#define HAUTEUR_MAPPING		400

using namespace cv;
using namespace std;

class WallFollowing : public CMOOSApp
{
	public:
		WallFollowing();
		~WallFollowing();

	protected:
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();
		
		void computeAndSendCommands(double angle, double distance);

	private: // Configuration variables

	private: // State variables
		unsigned int	m_iterations;
		double			m_timewarp;
		Mat				m_map;
		float 			m_angle_precedent;
		int				m_nb_data;
		list<pair<float,float> > 	m_obstacles;
    
    double m_values_min_distance;
    double m_values_max_distance;
    double m_values_min_angle;
    double m_values_max_angle;
    double m_gap_desired;
    double m_coeff_slide;
    double m_scale_display;
    int m_history_length;
		
		bool m_regulate;
		double m_current_heading, m_last_heading;
};

#endif 
