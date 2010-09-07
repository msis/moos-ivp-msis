/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Gaussian.h                                       */
/*    DATE: June 9th 2008                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/
 
#ifndef AOF_GAUSSIAN_HEADER
#define AOF_GAUSSIAN_HEADER

#include <vector>
#include <string>
#include "AOF.h"

/// This is just a renamed copy of moos-ivp's AOF_Gaussian class.  This class
/// exists to demonstrate how to code dynamically loadable AOFs.
class AOF_Gaussian2: public AOF {
 public:
  AOF_Gaussian2(IvPDomain domain) : AOF(domain) 
    {m_xcent=0; m_ycent=0; m_sigma=1; m_range=100;};
  ~AOF_Gaussian2() {};
  
 public:
  double evalPoint(const std::vector<double>& point) const;
  bool   setParam(const std::string&, double);

private:
  double m_xcent;  
  double m_ycent;
  double m_sigma;  
  double m_range;
};

extern "C" {
  AOF * createAOF(std::string name, IvPDomain domain) 
  {return new AOF_Gaussian2(domain);}
}

#endif
