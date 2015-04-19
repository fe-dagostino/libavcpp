/*
    This file is part of libav plus plus a wrapper for libav  (https://www.libav.org/) .

    libav++ is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libav++ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libav++.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AVFPS_H
#define AVFPS_H

#include "avcommon.h"
#include "FStopWatch.h"

USING_NAMESPACE_FED

namespace libavcpp
{
  
class CAVFps
{
public:
  /***/
  CAVFps();
  /***/
  CAVFps( double dSourceFps, double dWishedFps );

  /***/
  void  init( double dSourceFps, double dWishedFps );
  /***/
  inline bool   isValid() const
  { return (m_dFPSw > 0); }
  /***/
  inline double getSourceFps() const
  { return m_dFPSs; }
  /***/
  inline double getWishedFps() const
  { return m_dFPSw; }
  
  /***/
  bool 	bDuplicate() const;
  /***/
  bool 	bDrop() const;
  /***/
  void	reset();
  
private:
  double      m_dFPSs;  // source FPS
  double      m_dFPSw;  // wished FPS
  double      m_dFPSd;  // difference FPS
  double      m_dTime;
  FStopWatch  m_swTime;
};
  
} // namespace libavcpp

#endif // AVFPS_H
