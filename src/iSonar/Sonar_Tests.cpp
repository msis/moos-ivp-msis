#include <cstdlib>
#include <iostream>
#include "Sonar_Tests.h"

void launchTestsAndExitIfOk()
{
	Tests sessionDeTests("iSonar");
	sessionDeTests.afficherConclusionTests();
}