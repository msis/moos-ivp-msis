/**
 * \file main.cpp
 * \brief Programme principal de pJoystick
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 7th 2013
 *
 * Programme principal de lancement de l'application MOOS
 *
 */

#include <string>
#include "MBUtils.h"
#include "ColorParse.h"
#include "Joystick.h"
#include "Joystick_Info.h"

using namespace std;

int main(int argc, char *argv[])
{
	string mission_file;
	string run_command = argv[0];

	for(int i = 1; i < argc ; i++)
	{
		string argi = argv[i];
		
		if((argi=="-v") || (argi=="--version") || (argi=="-version"))
			showReleaseInfoAndExit();
			
		else if((argi=="-e") || (argi=="--example") || (argi=="-example"))
			showExampleConfigAndExit();
			
		else if((argi == "-h") || (argi == "--help") || (argi=="-help"))
			showHelpAndExit();
			
		else if((argi == "-i") || (argi == "--interface"))
			showInterfaceAndExit();
		
		else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
			mission_file = argv[i];
			
		else if(strBegins(argi, "--alias="))
			run_command = argi.substr(8);
			
		else if(i==2)
			run_command = argi;
	}

	if(mission_file == "")
		showHelpAndExit();
	
	else
	{
		cout << termColor("green");
		cout << "Lancement de " << run_command << endl;
		cout << termColor() << endl;

		Joystick Joystick;
		Joystick.Run(run_command.c_str(), mission_file.c_str());
	}

	return(0);
}

