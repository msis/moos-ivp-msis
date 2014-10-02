/************************************************************/
/**    NAME:                                               **/
/**    FILE: DrawDetections.h                                    **/
/**    DATE: December 29th, 1963                           **/
/************************************************************/

#ifndef DrawDetections_HEADER
#define DrawDetections_HEADER

#include <string>
#include "XYCircle.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

using namespace std;

class DrawDetections : public AppCastingMOOSApp
{
  public:
    DrawDetections();
    ~DrawDetections() {};

  protected: // Standard MOOSApp functions to overload  
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    bool Iterate();
    bool OnConnectToServer();
    bool OnStartUp();

  protected: // Standard AppCastingMOOSApp function to overload 
    bool buildReport();

  protected:
    void registerVariables();

  private: // State variables
    double m_nav_x, m_nav_y;
    int m_iteration;
};

#endif 
