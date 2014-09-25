/* ******************************************************** */
/*    ORGN:                                                 */
/*    FILE: WallFollowing_EchoSounder.h                     */
/*    DATE: April 2014                                      */
/*    AUTH: Simon Rohou                                     */
/* ******************************************************** */
 
#ifndef WallFollowing_EchoSounder_HEADER
#define WallFollowing_EchoSounder_HEADER

  #include <list>
  #include <cmath>
  #include <string>
  #include <cstdlib>
  #include <iostream>
  #include "AngleUtils.h"
  #include "GeomUtils.h"
  #include "IvPBehavior.h"
  #include "IvPDomain.h"
  #include "XYCircle.h"
  #include "XYPoint.h"
  #include "MOOS/libMOOS/MOOSLib.h"

  class IvPDomain;
  class WallFollowing_EchoSounder : public IvPBehavior
  {
    public:
      WallFollowing_EchoSounder(IvPDomain);
      ~WallFollowing_EchoSounder() {};
      
      IvPFunction*  onRunState();
      bool          setParam(std::string, std::string);
      void          onSetParamComplete();
      void          onRunToIdleState();

    protected:
      bool          updatePlatformInfo();
      IvPFunction*  IvPFunctionToFollowTheWall();

      void          postViewablePoints();
      void          postErasablePoints();
      
      double        m_max_speed;
      double        m_speed_coeff;
      double        m_dist_from_wall;
      double        m_dist_from_wall_desired;
  };

  #ifdef WIN32
    // Windows needs to explicitly specify functions to export from a dll
     #define IVP_EXPORT_FUNCTION __declspec(dllexport) 
  #else
     #define IVP_EXPORT_FUNCTION
  #endif

  extern "C"
  {
    IVP_EXPORT_FUNCTION IvPBehavior * createBehavior(std::string name, IvPDomain domain) 
    { return new WallFollowing_EchoSounder(domain); }
  }

#endif
