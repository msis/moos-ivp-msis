/************************************************************/
/**    NAME:                                               **/
/**    FILE: SauceCSV.h                                    **/
/**    DATE: December 29th, 1963                           **/
/************************************************************/

#ifndef SauceCSV_HEADER
#define SauceCSV_HEADER

#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

using namespace std;

class SauceCSV : public AppCastingMOOSApp
{
  public:
    SauceCSV();
    ~SauceCSV() {};

  protected: // Standard MOOSApp functions to overload  
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();

  protected: // Standard AppCastingMOOSApp function to overload 
    bool buildReport();

  protected:
    void registerVariables();

  private: // Configuration variables
    string m_logs_folder;
    string m_logs_file_name;

  private: // State variables
    double m_nav_x, m_nav_y, m_depth;
    double m_dbuptime;
    string m_mode;
    int m_logs_lines;
};

#endif 
