/************************************************************/
/*    NAME: MSIS                                              */
/*    ORGN: MIT                                             */
/*    FILE: CISCREA.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef CISCREA_HEADER
#define CISCREA_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "M6dbus.h"

class CISCREA : public AppCastingMOOSApp
{
	public:
		CISCREA();
		~CISCREA() {delete m_ciscrea;};

	protected: // Standard MOOSApp functions to overload  
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();

	protected: // Standard AppCastingMOOSApp function to overload 
		bool buildReport();

	protected:
		void registerVariables();
		void sendCommands();

	private: // Configuration variables
		std::string m_serial_port;
		int m_max_modbus_retries, m_max_turn_on_retries;
		
		int m_frri_lo_dz,m_reri_lo_dz,m_frle_lo_dz,
				m_rele_lo_dz,m_vert_lo_dz;
		int m_frri_up_dz,m_reri_up_dz,m_frle_up_dz,
				m_rele_up_dz,m_vert_up_dz;

		int m_frri_sens,m_reri_sens,m_frle_sens,
				m_rele_sens,m_vert_sens;

		int m_desired_thrust,m_desired_rudder,m_desired_slide,
				m_desired_elevator,m_front_lights;

		int m_depth_offset;

	private: // State variables
		M6dbus *m_ciscrea;
		bool connectToAUV();

};

#endif 
