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


#include "../include/avfps.h"

extern "C"
{
#include <libavutil/avutil.h>
}


namespace libavcpp
{

  
CAVFps::CAVFps()
{
  init( 0, 0 );
}

CAVFps::CAVFps( double dSourceFps, double dWishedFps )
{
  init( dSourceFps, dWishedFps );
}

void  CAVFps::init( double dSourceFps, double dWishedFps )
{
  m_dFPSs = dSourceFps;
  m_dFPSw = dWishedFps;
  
  m_dFPSd = (m_dFPSs>m_dFPSw)?(m_dFPSs-m_dFPSw):(m_dFPSw-m_dFPSs);
  m_dTime = 1 / m_dFPSd;
  
  m_swTime.Reset();
}
 
bool CAVFps::bDuplicate() const
{
  // If source FPS is greter than wished FPS then
  // surely duplicate is not required.
  if ( ( m_dFPSs >= m_dFPSw ) || (m_dFPSw < 0) )
    return false;
  
  return (m_swTime.Peek() >= m_dTime);
}

bool CAVFps::bDrop() const
{
  // If source FPS is lesser than wished FPS then
  // surely drop is not required.
  if ( ( m_dFPSw >= m_dFPSs ) || (m_dFPSw < 0) )
    return false;
  
  return (m_swTime.Peek() >= m_dTime);
}
  
void CAVFps::reset()
{
  m_swTime.Reset();
}
  
  
} // namespace libavcpp