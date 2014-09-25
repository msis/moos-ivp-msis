/* ******************************************************** */
/*    ORGN:                                                 */
/*    FILE: WallFollowing_EchoSounder.cpp                   */
/*    DATE: April 2014                                      */
/*    AUTH: Simon Rohou                                     */
/* ******************************************************** */

#ifdef _WIN32
  #pragma warning(disable : 4786)
  #pragma warning(disable : 4503)
#endif

#include "WallFollowing_EchoSounder.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Coupler.h"

using namespace std;


//-----------------------------------------------------------
// Procedure: Constructor

WallFollowing_EchoSounder::WallFollowing_EchoSounder(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  m_domain = subDomain(m_domain, "slide");
  this->setParam("name", "WallFollowing_EchoSounder");
  this->setParam("descriptor", "(d)WallFollowing_EchoSounder");  
  addInfoVars("ECHOSOUNDER_RAW_DISTANCE");
  
  m_max_speed = 1.0;
  m_speed_coeff = 1.0;
  m_dist_from_wall_desired = 3.0;
  m_dist_from_wall = m_dist_from_wall_desired;
}


//-----------------------------------------------------------
// Procedure: updatePlatformInfo

bool WallFollowing_EchoSounder::updatePlatformInfo() 
{
  bool ok;
  
  m_dist_from_wall = getBufferDoubleVal("ECHOSOUNDER_RAW_DISTANCE", ok);
  if(!ok)
  {
    string e_message = "updateInfo: ECHOSOUNDER_RAW_DISTANCE info not found.";
    postEMessage(e_message);
    cout << e_message << endl;
    return false;
  }
  
  return true;
}


//-----------------------------------------------------------
// Procedure: setParam

bool WallFollowing_EchoSounder::setParam(string param, string param_val) 
{ 
  if(IvPBehavior::setParam(param, param_val))
    return true;
    
  double dval = atof(param_val.c_str());
  bool non_neg_number = (isNumber(param_val) && (dval >= 0));
  
  if(param == "max_speed" && non_neg_number)
  {
    m_max_speed = dval;
    return true;
  }
  
  if(param == "dist_from_wall_desired" && non_neg_number)
  {
    m_dist_from_wall_desired = dval;
    return true;
  }
  
  if(param == "speed_coeff")
  {
    m_speed_coeff = dval;
    return true;
  }
    
  return false;
}


//-----------------------------------------------------------
// Procedure: onSetParamComplete

void WallFollowing_EchoSounder::onSetParamComplete() 
{
  
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *WallFollowing_EchoSounder::onRunState() 
{
  postViewablePoints();
  postMessage("PURSUIT", 1);
  updatePlatformInfo();

  /*IvPFunction *ipf = 0;
  ipf = IvPFunctionToFollowTheWall();
  
  if(ipf)
  {
    ipf->getPDMap()->normalize(0., 100.);
    
    if(false)
      ipf->setPWT(0.);
    
    else
      ipf->setPWT(m_priority_wt);
  }
  
  return ipf;*/
  return IvPFunctionToFollowTheWall();
}


//-----------------------------------------------------------
// Procedure: IvPFunctionWhenInsideIntRadius

IvPFunction* WallFollowing_EchoSounder::IvPFunctionToFollowTheWall()
{
  double error = m_dist_from_wall_desired - m_dist_from_wall;
  double desired_slide = error * m_speed_coeff;
  
  if(error < 0)
    desired_slide = max(-m_max_speed, desired_slide);
  
  else
    desired_slide = min(m_max_speed, desired_slide);
  
  postMessage("DIST_FROM_WALL_DESIRED", m_dist_from_wall_desired);
  postMessage("DIST_FROM_WALL", m_dist_from_wall);
  postMessage("DESIRED_SLIDE", desired_slide);
  
  ZAIC_PEAK slide_zaic(m_domain, "slide");
  // summit, pwidth, bwidth, delta, minutil, maxutil
  slide_zaic.setParams(desired_slide, 
                        m_max_speed * 0.1, 
                        m_max_speed * 0.2, 
                        20., 
                        0., 
                        100.);
  slide_zaic.setValueWrap(true);
  IvPFunction *slide_ipf = slide_zaic.extractIvPFunction();
  return slide_ipf;
}


//-----------------------------------------------------------
// Procedure: onRunToIdleState

void WallFollowing_EchoSounder::onRunToIdleState()
{
  postMessage("PURSUIT", 0);
  postErasablePoints();
}


//-----------------------------------------------------------
// Procedure: postViewablePoints

void WallFollowing_EchoSounder::postViewablePoints()
{
  
}


//-----------------------------------------------------------
// Procedure: postErasablePoints

void WallFollowing_EchoSounder::postErasablePoints()
{
  
}
