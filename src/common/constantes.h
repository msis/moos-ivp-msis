/**
 * \file constantes.h
 * \brief Constantes du projet
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 */

#ifndef CONSTANTES
#define CONSTANTES

#define TAILLE_MAX_X					120 * 100	// en cm
#define TAILLE_MAX_Y					50 * 100 	// en cm
#define TAILLE_MAX_Z					5.5 * 100 	// en cm

#define PRECISION_DONNEES_ENVOYEES		5.0 // en cm

//#define LARGEUR_IMAGE_CAMERA			320
//#define HAUTEUR_IMAGE_CAMERA			240
#define LARGEUR_IMAGE_CAMERA			360
#define HAUTEUR_IMAGE_CAMERA			288

#define HEADING_COTE_PISCINE_A			72.0
#define HEADING_COTE_PISCINE_B			HEADING_COTE_PISCINE_A + 90.0
#define HEADING_COTE_PISCINE_C			HEADING_COTE_PISCINE_B + 90.0
#define HEADING_COTE_PISCINE_D			HEADING_COTE_PISCINE_C + 90.0

#endif 
