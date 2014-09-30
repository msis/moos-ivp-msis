/* ******************************************************** */
/*    ORGN:                                                 */
/*    FILE: PipeInspection.h                                */
/*    DATE: April 2014                                      */
/*    AUTH: Simon Rohou                                     */
/* ******************************************************** */
 
#ifndef PipeInspection_HEADER
#define PipeInspection_HEADER

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
  class PipeInspection : public IvPBehavior
  {
    public:
      PipeInspection(IvPDomain);
      ~PipeInspection() {};
      
      IvPFunction*  onRunState();
      bool          setParam(std::string, std::string);
      void          onSetParamComplete();
      void          onRunToIdleState();

    protected:
      bool          updatePlatformInfo();
      IvPFunction*  IvPFunctionFocus();

      void          postViewablePoints();
      void          postErasablePoints();
      
      double        m_current_heading;
      double        m_desired_heading;
      double        m_current_depth;
      double        m_desired_depth;
      int           m_img_w;
      int           m_img_h;
      bool           m_pipe_is_visible;
      XYPoint        m_pipe_centroid_in_img;
      
      // Parameters
      float         m_inactive_area_w_ratio;
      float         m_inactive_area_h_ratio;
      float         m_ratio_fast_diving;
      float         m_delta_depth;
      float         m_delta_heading;
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
    { return new PipeInspection(domain); }
  }

#endif
