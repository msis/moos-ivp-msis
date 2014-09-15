/****************************************************************/
/*   NAME: MSIS                                             */
/*   ORGN: MIT Cambridge MA                                     */
/*   FILE: CISCREA_Info.cpp                               */
/*   DATE: Dec 29th 1963                                        */
/****************************************************************/

#include <cstdlib>
#include <iostream>
#include "CISCREA_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The iCISCREA application is used for               ");
  blk("                                                                ");
  blk("                                                                ");
  blk("                                                                ");
  blk("                                                                ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: iCISCREA file.moos [OPTIONS]                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch iCISCREA with the given process name         ");
  blk("      rather than iCISCREA.                           ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of iCISCREA.        ");
  blk("                                                                ");
  blk("Note: If argv[2] does not otherwise match a known option,       ");
  blk("      then it will be interpreted as a run alias. This is       ");
  blk("      to support pAntler launching conventions.                 ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("iCISCREA Example MOOS Configuration                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = iCISCREA                              ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  //Serial port ");
  blk("  SERIAL_PORT = /dev/ttyUSB0");
  blk("                                                                ");
  blk("  // Max number of retries before giving up");
  blk("  MAX_MODBUS_RETRIES = 10");
  blk("  MAX_TURNON_RETRIES = 10");
  blk("                                                                ");
  blk("  // Lower and upper bound for motors deadzone");
  blk("  FRRI_LO_DZ = 100");
  blk("  RERI_LO_DZ = 100");
  blk("  RELE_LO_DZ = 100");
  blk("  FRLE_LO_DZ = 100");
  blk("  VERT_LO_DZ = 100");
  blk("  FRRI_UP_DZ = 130");
  blk("  RERI_UP_DZ = 130");
  blk("  RELE_UP_DZ = 130");
  blk("  FRLE_UP_DZ = 130");
  blk("  VERT_UP_DZ = 130");
  blk("                                                                ");
  blk("  // Motors' sens of rotation (when the motor value is positive, motors should push water away from the body.");
  blk("  FRRI_SENS = 1");
  blk("  RERI_SENS = 1");
  blk("  RELE_SENS = 1");
  blk("  FRLE_SENS = 1");
  blk("  VERT_SENS = 1");
  blk("                                                                ");
  blk("  DEPTH_OFFSET = 0.85");
  blk("                                                                ");
  blk("                                                                ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("iCISCREA INTERFACE                                    ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  DESIRED_THRUST     = [-100,100] // If positive, moves forward.");
  blk("  DESIRED_SLIDE      = [-100,100] // If positive, slides to the right.");
  blk("  DESIRED_RUDDER     = [-100,100] // If positive, turns to the right.");
  blk("  DESIRED_ELEVATOR   = [-100,100] // If positive, dives.");
  blk("  FRONT_LIGHTS       = [0,100] // Linear (on PWM) from OFF to full ON.");
  blk("                                                                ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  Publications are determined by the node message content.      ");
  blk("  CISCREA_ON       = true/false");
  blk("  DEPTH            = value in meters.");
  blk("  CURRENT_BATT1    = Current of battery 1 in mA.");
  blk("  CURRENT_BATT2    = Current of battery 2 in mA.");
  blk("  VOLTAGE_BATT1    = Current of battery 1 in V.");
  blk("  VOLTAGE_BATT2    = Current of battery 2 in V.");
  blk("  CONSUMPTION_BATT1    = Consumption of battery 1 in W.");
  blk("  CONSUMPTION_BATT2    = Consumption of battery 2 in W.");
  blk("  HEADING          = Heading.");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("iCISCREA", "gpl");
  exit(0);
}

