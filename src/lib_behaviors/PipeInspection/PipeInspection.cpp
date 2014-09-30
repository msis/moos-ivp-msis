/* ******************************************************** */
/*    ORGN:                                                 */
/*    FILE: PipeInspection.cpp                              */
/*    DATE: April 2014                                      */
/*    AUTH: Simon Rohou                                     */
/* ******************************************************** */

#ifdef _WIN32
  #pragma warning(disable : 4786)
  #pragma warning(disable : 4503)
#endif

#include "PipeInspection.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;


//-----------------------------------------------------------
// Procedure: Constructor

PipeInspection::PipeInspection(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  m_domain = subDomain(m_domain, "course,depth");
  this->setParam("name", "PipeInspection");
  this->setParam("descriptor", "(d)PipeInspection");
  addInfoVars("PIPE_DETECTION");
  addInfoVars("DEPTH");
  addInfoVars("HEADING");

  m_img_w = 360;
  m_img_h = 288;
  
  m_inactive_area_w_ratio = 0.1;
  m_inactive_area_h_ratio = 0.1;
  
  m_delta_depth = 0.10;
  m_delta_heading = 3.;
  
  m_current_depth = 0.;
  m_current_heading = 0.;
  
  m_ratio_fast_diving = 3.;
}


//-----------------------------------------------------------
// Procedure: updatePlatformInfo

bool PipeInspection::updatePlatformInfo() 
{
  bool ok;
  
  m_current_heading = getBufferDoubleVal("HEADING", ok);
  if(!ok)
  {
    string e_message = "updateInfo: HEADING info not found.";
    postEMessage(e_message);
    cout << e_message << endl;
    return false;
  }
  
  m_current_depth = getBufferDoubleVal("DEPTH", ok);
  if(!ok)
  {
    string e_message = "updateInfo: DEPTH info not found.";
    postEMessage(e_message);
    cout << e_message << endl;
    return false;
  }
  
  string m_moosvar_pipe_detection = getBufferStringVal("PIPE_DETECTION", ok);
  postMessage("PIPEINSP_MOOSVAR", m_moosvar_pipe_detection);
  if(!ok)
  {
    string e_message = "updateInfo: PIPE_DETECTION info not found.";
    postEMessage(e_message);
    cout << e_message << endl;
    return false;
  }
  
  if(m_moosvar_pipe_detection == "false")
    m_pipe_is_visible = false;
  
  else
  {
    float x_value, y_value;
    
    MOOSValFromString(x_value, m_moosvar_pipe_detection, "x", true);
    MOOSValFromString(y_value, m_moosvar_pipe_detection, "y", true);
    MOOSValFromString(m_img_w, m_moosvar_pipe_detection, "img_w", true);
    MOOSValFromString(m_img_h, m_moosvar_pipe_detection, "img_h", true);
    
    m_pipe_centroid_in_img.set_vx(x_value);
    m_pipe_centroid_in_img.set_vy(y_value);
    
    m_pipe_is_visible = true;
  }
  
  return true;
}


//-----------------------------------------------------------
// Procedure: setParam

bool PipeInspection::setParam(string param, string param_val) 
{ 
  if(IvPBehavior::setParam(param, param_val))
    return true;
    
  double dval = atof(param_val.c_str());
  bool non_neg_number = (isNumber(param_val) && (dval >= 0));
  
  if(param == "inactive_area_w_ratio" && non_neg_number)
  {
    m_inactive_area_w_ratio = dval;
    return true;
  }
  
  if(param == "inactive_area_h_ratio" && non_neg_number)
  {
    m_inactive_area_h_ratio = dval;
    return true;
  }
  
  if(param == "delta_heading" && non_neg_number)
  {
    m_delta_heading = dval;
    return true;
  }
  
  if(param == "delta_depth" && non_neg_number)
  {
    m_delta_depth = dval;
    return true;
  }
  
  if(param == "ratio_fast_diving" && non_neg_number)
  {
    m_ratio_fast_diving = dval;
    return true;
  }
  
  return false;
}


//-----------------------------------------------------------
// Procedure: onSetParamComplete

void PipeInspection::onSetParamComplete() 
{
  
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *PipeInspection::onRunState() 
{
  postViewablePoints();
  postMessage("PURSUIT", 1);
  updatePlatformInfo();

  IvPFunction *ipf = 0;
  ipf = IvPFunctionFocus();
  
  /*if(ipf)
  {
    ipf->getPDMap()->normalize(0., 100.);
    
    if(false)
      ipf->setPWT(0.);
    
    else
      ipf->setPWT(m_priority_wt);
  }*/
  
  return ipf;
}


//-----------------------------------------------------------
// Procedure: IvPFunctionFocus

IvPFunction* PipeInspection::IvPFunctionFocus()
{
  if(!m_pipe_is_visible)
  {
    m_desired_heading = m_current_heading;
    m_desired_depth = m_current_depth + m_ratio_fast_diving * m_delta_depth;
  }
  
  else
  {
    // Along X: Heading
    if(m_pipe_centroid_in_img.x() < (-1. * m_inactive_area_w_ratio * m_img_w / 2.))
      m_desired_heading = m_current_heading - m_delta_heading;
    
    else if(m_pipe_centroid_in_img.x() > (1. * m_inactive_area_w_ratio * m_img_w / 2.))
      m_desired_heading = m_current_heading + m_delta_heading;
    
    else
      m_desired_heading = m_current_heading;
      
    // Along Y: Depth
    if(m_pipe_centroid_in_img.y() < (-1. * m_inactive_area_h_ratio * m_img_h / 2.))
      m_desired_depth = m_current_depth - m_delta_depth;
    
    else if(m_pipe_centroid_in_img.y() > (1. * m_inactive_area_h_ratio * m_img_h / 2.))
      m_desired_depth = m_current_depth + m_delta_depth;
    
    else
      m_desired_depth = m_current_depth;
  }
  
  postMessage("PIPEINSP_DSRD_HEADING", m_desired_heading);
  postMessage("PIPEINSP_DSRD_DEPTH", m_desired_depth);
  postMessage("MIN_X", -1. * m_inactive_area_w_ratio * m_img_w / 2.);
  postMessage("MAX_X", 1. * m_inactive_area_w_ratio * m_img_w / 2.);
  postMessage("MIN_Y", -1. * m_inactive_area_h_ratio * m_img_h / 2.);
  postMessage("MAX_Y", 1. * m_inactive_area_h_ratio * m_img_h / 2.);
  
  // Heading
    ZAIC_PEAK course_zaic(m_domain, "course");
    // summit, pwidth, bwidth, delta, minutil, maxutil
    course_zaic.setParams(m_desired_heading, 
                          2.0, 
                          4.0, 
                          20., 
                          0., 
                          100.);
    course_zaic.setValueWrap(true);
    IvPFunction *course_ipf = course_zaic.extractIvPFunction();
    
  // Depth
    ZAIC_PEAK depth_zaic(m_domain, "depth");
    // summit, pwidth, bwidth, delta, minutil, maxutil
    depth_zaic.setParams(1.0, 
                          2.0, 
                          4.0, 
                          20., 
                          0., 
                          100.);
    IvPFunction *depth_ipf = depth_zaic.extractIvPFunction();
    
  // Coupling
    OF_Coupler coupler;
    return coupler.couple(course_ipf, depth_ipf);
}


//-----------------------------------------------------------
// Procedure: onRunToIdleState

void PipeInspection::onRunToIdleState()
{
  postMessage("PURSUIT", 0);
  postErasablePoints();
}


//-----------------------------------------------------------
// Procedure: postViewablePoints

void PipeInspection::postViewablePoints()
{
  
}


//-----------------------------------------------------------
// Procedure: postErasablePoints

void PipeInspection::postErasablePoints()
{
  
}
