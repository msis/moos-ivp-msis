/****************************************************************/
/*   NAME:                                              */
/*   ORGN: MIT Cambridge MA                                     */
/*   FILE: IMU_RazorAHRS_Info.cpp                               */
/*   DATE: Dec 29th 1963                                        */
/****************************************************************/

#include <cstdlib>
#include <iostream>
#include "IMU_RazorAHRS_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The iIMU_RazorAHRS application is used for               ");
  blk("  reading data from Sparkfun RAZOR 9DOF AHRS.                   ");
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
  blu("Usage: iIMU_RazorAHRS file.moos [OPTIONS]                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch iIMU_RazorAHRS with the given process name         ");
  blk("      rather than iIMU_RazorAHRS.                           ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of iIMU_RazorAHRS.        ");
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
  blu("iIMU_RazorAHRS Example MOOS Configuration                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = iIMU_RazorAHRS                              ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  SERIAL_PORT = /dev/ttyUSB0                                    ");
  blk("  MODE = ANGLES_CALIBRATED_SENSOR_DATA                          ");
  blk("         // RAW_SENSOR_DATA                                     ");
  blk("         // CALIBRATED_SENSOR_DATA                              ");
  blk("         // ANGLES_CALIBRATED_SENSOR_DATA                       ");
  blk("         // ANGLES_RAW_SENSOR_DATA                              ");
  blk("         // ANGLES                                              ");
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
  blu("iIMU_RazorAHRS INTERFACE                                    ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("                                                                ");
  blk("                                                                ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk(" Variables are only published when the corresponding            ");
  blk(" mode is active!                                                ");
  blk("                                                                ");
  blk("  YAW = read yaw value                                          ");
  blk("  PITCH = read pitch value                                      ");
  blk("  ROLL = read yaw value                                         ");
  blk("  ACC_X = Acceleration according to x                           ");
  blk("  ACC_Y = Acceleration according to y                           ");
  blk("  ACC_Z = Acceleration according to z                           ");
  blk("  MAG_X = Magnetude according to x                              ");
  blk("  MAG_Y = Magnetude according to y                              ");
  blk("  MAG_Z = Magnetude according to z                              ");
  blk("  GYRO_X = Rotation according to x                              ");
  blk("  GYRO_Y = Rotation according to y                              ");
  blk("  GYRO_Z = Rotation according to z                              ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("iIMU_RazorAHRS", "gpl");
  exit(0);
}

