/**
 * \file main.cpp
 * \brief Programme principal de pEchoSounder
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Programme principal de lancement de l'application MOOS
 *
 */

#include <string>
#include "MBUtils.h"
#include "ColorParse.h"
#include "EchoSounder.h"
#include "EchoSounder_Info.h"
#include "EchoSounder_Tests.h"

using namespace std;

int main(int argc, char *argv[])
{
	string mission_file;
	bool lancer_tests = false;
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
			
		else if((argi == "-tests"))
			lancer_tests = true;
		
		else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
			mission_file = argv[i];
			
		else if(strBegins(argi, "--alias="))
			run_command = argi.substr(8);
			
		else if(i==2)
			run_command = argi;
	}

	if(mission_file == "")
		showHelpAndExit();

	if(lancer_tests)
		launchTestsAndExitIfOk();
	
	else
	{
		cout << termColor("green");
		cout << "Lancement de " << run_command << endl;
		cout << termColor() << endl;

		EchoSounder EchoSounder;
		EchoSounder.Run(run_command.c_str(), mission_file.c_str());
	}

	return(0);
}

