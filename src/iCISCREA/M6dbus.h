/**
 * \file M6dbus.h
 * \brief Classe M6dbus
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Classe permettant la manipulation de l'AUV par lecture et écriture dans ses registres
 * Utilisation du protocole de communication Modbus
 *
 */

#ifndef M6DBUS_H
#define M6DBUS_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "modbus/modbus.h"
#ifdef _WIN32
	#include "MOOS/libMOOS/Utils/MOOSNTSerialPort.h"
#else
	#include "MOOS/libMOOS/Utils/MOOSLinuxSerialPort.h"
#endif

#define Offcet 0x1A
#define MAX_RETRIES	10

using namespace std;

class M6dbus
{
	public:
		explicit M6dbus(string IP, int prt);
		explicit M6dbus(string SerialPort);
		~M6dbus();

		bool getOn();

		void setMaxRetries(int max);

		int updatePropulsors();
		int updateDepthAndDirec();
		int updateRegTab(uint16_t*);
		int writeReg(int,int);
		int getRegNum(int);

		int updateAll();

		int turnLightOn(int);
		int turnLightOn(int,int);
		int turnLightOff();
		int turnLightOff(int);

		int turnLaserOn(int);
		int turnLaserOff(int);

		int setAutoDepth();
		int setAutoDirec();
		int setAutoAltit();
		int setAutoProxm();

		int resetCountAh();

		int setModePower();
		int setModeLogic();

		int setCamera(int);
		int setCamLights(int);
		int clrCamLights(int);
		int setCameraBUP(int);
		int setCameraBDO(int);
		int setCameraBLE(int);
		int setCameraBRI(int);
		int setCameraBSE(int);
		int setCamTilt(int);

		int getClockR4Wr();
		int getClockR4Rd();
		int* getAuvTime();
		int setAuvTime(int[4]);

		int getAlarms();
		bool getWaterIn1();
		bool getWaterIn2();
		bool getWaterIn3();
		bool getWaterInCam();
		bool getHiTempIn();
		bool getLowU1();
		bool getLowU2();
		bool getHighI1();
		bool getHighI2();
		bool getMaxDepth();
		bool getErrRConf();
		bool getErrI2C();
		bool getErrSPI();
		bool getErrLink();

		int getDepth();
		int getDirec();
		int getU1();
		int getU2();
		int getI1();
		int getI2();
		int getTempMB();
		int getTempOut();
		int getTempEx1();
		int getTempEx2();
		int getTempEx3();
		int getCapBat1();
		int getCapBat2();
		int getVersion();

		int setPropFrRi(int);
		int setPropFrLe(int);
		int setPropReRi(int);
		int setPropReLe(int);
		int setPropVert(int);

	private:
		modbus_t *Modbus;
		bool on;
		int maxRetries;
		uint16_t regTab[48];
		//Variables (raw value)
		int depth, direction, U1, I1, U2, I2, tempMB,
		tempOut, tempEx1, tempEx2, tempEx3, capBat1, capBat2, version;
		int auvTime[4];
		//Alarms
		int alarms;
		bool waterIn1, waterIn2, waterIn3, waterInC, hiTempIn, lowU1, lowU2,
		highI1, highI2, maxDepth, errRConf, errI2C, errSPI, errLink;
		uint16_t m_RegPropFr, m_RegPropRe, m_RegPropVert;
};

#endif // M6DBUS_H
