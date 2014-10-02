/**
 * \file WallFollowing.cpp
 * \brief Classe WallFollowing
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 25th 2013
 *
 * Application MOOS de suivi de mur
 *
 */

#include <iterator>
#include "MBUtils.h"
#include "WallFollowing.h"

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
WallFollowing::WallFollowing()
{
	m_iterations = 0;
	m_timewarp   = 1;
	m_nb_data = 0;
	m_angle_precedent = 360;
	
	cvNamedWindow("Mapping", 1);
	m_map = Mat::zeros(LARGEUR_MAPPING, HAUTEUR_MAPPING, CV_8UC3);
	m_regulate = false;
	m_regulate = true;
  m_activation = true;
  
  m_activation_var = "ACTIVATION_WF";
  m_history_length = 80;
  
  /*m_obstacles.push_back(make_pair(80., 2.5));
  m_obstacles.push_back(make_pair(66., 2.7));
  m_obstacles.push_back(make_pair(64., 2.2));
  m_obstacles.push_back(make_pair(58., 2.8));
  m_obstacles.push_back(make_pair(54., 3.5));
  m_obstacles.push_back(make_pair(50., 3.0));
  m_obstacles.push_back(make_pair(42., 3.3));
  m_obstacles.push_back(make_pair(38., 3.7));
  m_obstacles.push_back(make_pair(30., 4.1));
  m_obstacles.push_back(make_pair(25., 4.6));
  m_obstacles.push_back(make_pair(22., 4.4));
  m_obstacles.push_back(make_pair(22., 5.));*/
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

WallFollowing::~WallFollowing()
{
	Mat map = Mat::zeros(LARGEUR_MAPPING, HAUTEUR_MAPPING, CV_8UC3);
	cvDestroyWindow("Mapping");
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool WallFollowing::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin() ; p != NewMail.end() ; p++)
	{
		CMOOSMsg &msg = *p;
/*                if (msg.GetSource() == this->GetAppName())
                  continue;
*/
		  /*
		if(msg.GetKey() == "WF_START" && msg.GetAsString()!="")
		{
		  m_regulate = true;
		  Notify("WF_START", "");
		}
		if(msg.GetKey() == "WF_STOP" && msg.GetAsString()!="")
		{
		  m_regulate = false;
		  Notify("WF_STOP", "");
		}*/
		if(msg.GetKey() == "YAW")
		{
			m_last_heading = m_current_heading;
			
		  m_current_heading = MOOSDeg2Rad(msg.GetDouble());
		  double a = MOOSDeg2Rad(-12.6), b = 0.45, c = MOOSDeg2Rad(-10.5);
		  m_current_heading = MOOSRad2Deg(m_current_heading - ( a*sin(m_current_heading+b) + c));
			
			for(list<pair<float, float> >::iterator it = m_obstacles.begin() ; it != m_obstacles.end() ; it ++)
			{
				it->first += (m_current_heading-m_last_heading); 		// clef
			}
		}
    
		if(msg.GetKey() == "SONAR_RAW_DATA")
		{
			float angle = 0;
			int nRows, nCols;
			float ad_interval = 0.25056;
			vector<unsigned int> scanline;
			MOOSValFromString(angle, msg.GetString(), "bearing");
			MOOSValFromString(ad_interval, msg.GetString(), "ad_interval");
			MOOSValFromString(scanline, nRows, nCols, msg.GetString(), "scanline");
			
      if(angle360(angle) > min(m_values_min_angle, m_values_max_angle) && angle360(angle) < max(m_values_min_angle, m_values_max_angle))
      {
        // Récupération du max de la scanline
        float distance = 0, distance_obstacle_max = 0, distance_mini = 1.0;
        int obstacle_max = 0;
        
        for(int i = 0 ; i < (int)scanline.size() ; i ++)
        {
          if((int)scanline[i] > obstacle_max)
          {
            distance = ad_interval / 2.0 * i;
            
            if(distance > m_values_min_distance && distance < m_values_max_distance)
            {
              distance_obstacle_max = distance;
              obstacle_max = scanline[i];
            }
          }
        }
        
        m_obstacles.push_back(make_pair(angle/* - 90. + m_current_heading*/, distance_obstacle_max));
      }
      
			while(m_obstacles.size() > m_history_length)
			{
				m_obstacles.pop_front();
			}
		}
    
		if(msg.GetKey() == m_activation_var)
		{
      m_activation = (msg.GetString() == "true");
    }
		  
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
	}

	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool WallFollowing::OnConnectToServer()
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
 
bool WallFollowing::Iterate()
{
	float angle = 0.0, coefficient_affichage = m_scale_display/*45.0/*8.*/;
	float distance = 0.0, taille_pointeur;
	m_iterations++;

	m_map = Mat::zeros(LARGEUR_MAPPING, HAUTEUR_MAPPING, CV_8UC3);
	m_map = Scalar(255, 255, 255);
	
	std::vector<Point2f> points_obstacles;
	
	for(list<pair<float, float> >::const_iterator it = m_obstacles.begin() ; it != m_obstacles.end() ; it ++)
	{
		angle = it->first; 		// clef
		distance = it->second; 	// valeur
		
		//if(distance < 5.)
		/*if(distance < m_values_min_distance || distance > m_values_max_distance)
			continue;*/
      
		float x_obstacle = 0;
		float y_obstacle = 0;

		y_obstacle -= distance * cos(angle * M_PI / 180.0);
		x_obstacle += distance * sin(angle * M_PI / 180.0);
    
		// Filtrage des angles
		double angle_degre = MOOSRad2Deg(MOOS_ANGLE_WRAP(MOOSDeg2Rad(angle)));
    //cout << angle360(angle_degre) << " \t" << angle360(m_values_min_angle) << " \t" << angle360(m_values_max_angle) << endl;
		if(angle360(angle_degre) > min(m_values_min_angle, m_values_max_angle) && angle360(angle_degre) < max(m_values_min_angle, m_values_max_angle))
		{
			points_obstacles.push_back(Point2f(x_obstacle, y_obstacle));
			
			x_obstacle *= -coefficient_affichage;
			y_obstacle *= coefficient_affichage;
			
			x_obstacle += LARGEUR_MAPPING / 2.0;
			y_obstacle += HAUTEUR_MAPPING / 2.0;
			
			// Pointeurs de données
			taille_pointeur = 3;
			line(m_map, Point(x_obstacle, y_obstacle - taille_pointeur), Point(x_obstacle, y_obstacle + taille_pointeur), Scalar(161, 149, 104), 1, 8, 0);
			line(m_map, Point(x_obstacle - taille_pointeur, y_obstacle), Point(x_obstacle + taille_pointeur, y_obstacle), Scalar(161, 149, 104), 1, 8, 0);
		}
	}
	
	int echelle_ligne = 150;
	Mat m(points_obstacles);
	
	if(!points_obstacles.empty())
	{
		Vec4f resultat_regression;
		
		try
		{
			// Méthode 1
			fitLine(m, resultat_regression, CV_DIST_L2, 0, 0.01, 0.01);
			float x0 = resultat_regression[2];
			float y0 = resultat_regression[3];
			float vx = resultat_regression[0];
			float vy = resultat_regression[1];
			// Affichage de l'approximation
			line(m_map, 
					Point((LARGEUR_MAPPING / 2.0) - (x0 * coefficient_affichage) + (vx * echelle_ligne), 
							(HAUTEUR_MAPPING / 2.0) + (y0 * coefficient_affichage) - (vy * echelle_ligne)),
					Point((LARGEUR_MAPPING / 2.0) - (x0 * coefficient_affichage) - (vx * echelle_ligne), 
							(HAUTEUR_MAPPING / 2.0) + (y0 * coefficient_affichage) + (vy * echelle_ligne)),
					Scalar(29, 133, 217), 1, 8, 0); // Orange
					
			// Méthode 2
			fitLine(m, resultat_regression, CV_DIST_L12, 0, 0.01, 0.01);
			x0 = resultat_regression[2];
			y0 = resultat_regression[3];
			vx = resultat_regression[0];
			vy = resultat_regression[1];
			// Affichage de l'approximation
			line(m_map, 
					Point((LARGEUR_MAPPING / 2.0) - (x0 * coefficient_affichage) + (vx * echelle_ligne), 
							(HAUTEUR_MAPPING / 2.0) + (y0 * coefficient_affichage) - (vy * echelle_ligne)),
					Point((LARGEUR_MAPPING / 2.0) - (x0 * coefficient_affichage) - (vx * echelle_ligne), 
							(HAUTEUR_MAPPING / 2.0) + (y0 * coefficient_affichage) + (vy * echelle_ligne)),
					Scalar(77, 130, 27), 1, 8, 0); // Vert
					
			// Méthode 3
			fitLine(m, resultat_regression, CV_DIST_L1, 0, 0.01, 0.01);
			x0 = resultat_regression[2];
			y0 = resultat_regression[3];
			vx = resultat_regression[0];
			vy = resultat_regression[1];
			// Affichage de l'approximation
			line(m_map, 
					Point((LARGEUR_MAPPING / 2.0) - (x0 * coefficient_affichage) + (vx * echelle_ligne), 
							(HAUTEUR_MAPPING / 2.0) + (y0 * coefficient_affichage) - (vy * echelle_ligne)),
					Point((LARGEUR_MAPPING / 2.0) - (x0 * coefficient_affichage) - (vx * echelle_ligne), 
							(HAUTEUR_MAPPING / 2.0) + (y0 * coefficient_affichage) + (vy * echelle_ligne)),
					Scalar(13, 13, 188), 1, 8, 0); // Rouge
			// Affichage de l'origine
			taille_pointeur = 6;
			line(m_map, 
					Point((LARGEUR_MAPPING / 2.0) - (x0 * coefficient_affichage), 
							(HAUTEUR_MAPPING / 2.0) + (y0 * coefficient_affichage) - taille_pointeur),
					Point((LARGEUR_MAPPING / 2.0) - (x0 * coefficient_affichage), 
							(HAUTEUR_MAPPING / 2.0) + (y0 * coefficient_affichage) + taille_pointeur),
					Scalar(9, 0, 130), 2, 8, 0);
			line(m_map, 
					Point((LARGEUR_MAPPING / 2.0) - (x0 * coefficient_affichage) - taille_pointeur, 
							(HAUTEUR_MAPPING / 2.0) + (y0 * coefficient_affichage)),
					Point((LARGEUR_MAPPING / 2.0) - (x0 * coefficient_affichage) + taille_pointeur, 
							(HAUTEUR_MAPPING / 2.0) + (y0 * coefficient_affichage)),
					Scalar(9, 0, 130), 2, 8, 0);
			
			angle = atan2(vy, vx);
			cout << "X0 : " << x0 << "\t\tY0 : " << y0 << endl;
			distance = abs(-vy*x0 + vx*y0);
			cout << "Angle : " << angle * 180.0 / M_PI << "\t\tDist : " << distance << endl;
			m_Comms.Notify("DIST_MUR", distance);

			if(m_regulate)
				computeAndSendCommands(angle, distance);
		}
		
		catch(Exception e) { }
		
		// Rotation
		//Point2f src_center(m_map.cols/2.0F, m_map.rows/2.0F);
		//Mat rot_mat = getRotationMatrix2D(src_center, 180.0, 1.0);
		//warpAffine(m_map, m_map, rot_mat, m_map.size());
	}
		
	// Affichage des échelles circulaires
	char texte[50];
	float taille_texte = 0.4;
	Scalar couleur_echelles(220, 220, 220);
	for(float j = 1.0 ; j < 30.0 ; j ++)
	{
		float rayon = coefficient_affichage * j;
		circle(m_map, Point(LARGEUR_MAPPING / 2, HAUTEUR_MAPPING / 2), rayon, couleur_echelles, 1);
		sprintf(texte, "%dm", (int)j);
		rayon *= cos(M_PI / 4.0);
		putText(m_map, string(texte), Point((LARGEUR_MAPPING / 2) + rayon, (HAUTEUR_MAPPING / 2) - rayon), FONT_HERSHEY_SIMPLEX, taille_texte, couleur_echelles);
	}
	
	// Affichage de l'origine
	taille_pointeur = 20;
	line(m_map, Point(LARGEUR_MAPPING / 2, HAUTEUR_MAPPING / 2 - taille_pointeur * 1.5), Point(LARGEUR_MAPPING / 2, HAUTEUR_MAPPING / 2 + taille_pointeur), Scalar(150, 150, 150), 1, 8, 0);
	line(m_map, Point(LARGEUR_MAPPING / 2 - taille_pointeur, HAUTEUR_MAPPING / 2), Point(LARGEUR_MAPPING / 2 + taille_pointeur, HAUTEUR_MAPPING / 2), Scalar(150, 150, 150), 1, 8, 0);
	
	// Localisation des points de données
	line(m_map, Point(LARGEUR_MAPPING / 2 + LARGEUR_MAPPING*cos(MOOSDeg2Rad(m_values_min_angle+90.)), HAUTEUR_MAPPING / 2 - LARGEUR_MAPPING*sin(MOOSDeg2Rad(m_values_min_angle+90.))), Point(LARGEUR_MAPPING / 2, HAUTEUR_MAPPING / 2), Scalar(150, 150, 150), 1, 8, 0);
	line(m_map, Point(LARGEUR_MAPPING / 2 + LARGEUR_MAPPING*cos(MOOSDeg2Rad(m_values_max_angle+90.)), HAUTEUR_MAPPING / 2 - LARGEUR_MAPPING*sin(MOOSDeg2Rad(m_values_max_angle+90.))), Point(LARGEUR_MAPPING / 2, HAUTEUR_MAPPING / 2), Scalar(150, 150, 150), 1, 8, 0);
	
	// Affichage d'informations
	if(!points_obstacles.empty())
	{
		sprintf(texte, "Dist = %.2fm   Angle = %.2f", distance, MOOSRad2Deg(angle));
		putText(m_map, string(texte), Point(10, HAUTEUR_MAPPING - 10), FONT_HERSHEY_SIMPLEX, taille_texte, Scalar(50, 50, 50));
	}
	
	imshow("Mapping", m_map);
	waitKey(1);
	
	return(true);
}



/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool WallFollowing::OnStartUp()
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

			if(param == "VALUES_MIN_DISTANCE")
        m_values_min_distance = atof(value.c_str());

			else if(param == "VALUES_MAX_DISTANCE")
        m_values_max_distance = atof(value.c_str());

			else if(param == "VALUES_MIN_ANGLE")
        m_values_min_angle = angle360(atof(value.c_str()) - 90.);

			else if(param == "VALUES_MAX_ANGLE")
        m_values_max_angle = angle360(atof(value.c_str()) - 90.);
			
			else if(param == "GAP_DESIRED")
        m_gap_desired = atof(value.c_str());
			
			else if(param == "COEFF_SLIDE")
        m_coeff_slide = atof(value.c_str());
			
			else if(param == "SCALE_DISPLAY")
        m_scale_display = atof(value.c_str());
			
			else if(param == "HISTORY_LENGTH")
        m_history_length = atoi(value.c_str());
			
			else if(param == "ACTIVATION_VAR")
      {
        m_activation_var = value;
        m_Comms.Register(m_activation_var);
      }
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
 
void WallFollowing::RegisterVariables()
{
	m_Comms.Register("SONAR_RAW_DATA", 0);
	m_Comms.Register("YAW", 0);
}


/**
 * \fn
 * \brief Calcule et publie sur la MOOSDB les commandes pour le wall following
 */
 
void WallFollowing::computeAndSendCommands(double angle, double distance)
{
	const double m_dist_consigne = m_gap_desired;
	const double k_d = m_coeff_slide;
	const double max_uy = 20.0;

	double err_d =  m_dist_consigne - distance;
	double u_y = -k_d * err_d;
	u_y = max(-max_uy, min(max_uy, u_y));

	const double k_h = 2.0;
	const double max_uh = 5.0;

	//double u_h = k_h * cos(angle);
	//u_h = max(-max_uh, min(max_uh, u_h));
	double angle_cadrant =  acos(cos(angle));
	double u_h = -0.25 * (MOOSRad2Deg(angle_cadrant) - 90.);

  float arrow_length = 50.;
  float desired_heading = m_current_heading - 90. + MOOSRad2Deg(angle_cadrant);
  
  line(m_map, Point(LARGEUR_MAPPING / 2, HAUTEUR_MAPPING / 2), Point(LARGEUR_MAPPING / 2 + cos(angle_cadrant) * arrow_length, HAUTEUR_MAPPING / 2 + sin(angle_cadrant) * arrow_length), Scalar(173, 127, 68), 2, 8, 0);
  line(m_map, Point(LARGEUR_MAPPING / 2, HAUTEUR_MAPPING / 2), Point(LARGEUR_MAPPING / 2 + cos(MOOSDeg2Rad(m_current_heading)) * arrow_length, HAUTEUR_MAPPING / 2 + sin(MOOSDeg2Rad(m_current_heading)) * arrow_length), Scalar(68, 127, 173), 1, 8, 0);
  
	if(m_activation)
  {
    Notify("DESIRED_HEADING", desired_heading);
    Notify("DESIRED_SLIDE", u_y);
  }
  
	cout << "CURRENT_HEADING: " << m_current_heading << endl;
  cout << "DESIRED_HEADING: " << desired_heading << endl;
  cout << "DESIRED_SLIDE: " << u_y << endl;
}
