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


#ifndef AVCOLOR_H
#define AVCOLOR_H

#include "avcommon.h"
#include <sys/types.h>

namespace libavcpp
{
  
class CAVColor
{
public:
  /**
   * Initialize object instance with white color. 
   */
  CAVColor();
  /**
   * Copy constructor. 
   */
  CAVColor( const CAVColor& cr );
  /**
   * Initialize object instance with specified rgba values.
   */
  CAVColor( u_int8_t r, u_int8_t g, u_int8_t b, u_int8_t a );
  
  /***/
  virtual ~CAVColor();

  /**
   * Update object values with specified rgba values.
   */
  inline AVResult    set( u_int8_t r, u_int8_t g, u_int8_t b, u_int8_t a )
  {
    m_color = a << 24 | r << 16 | g << 8 | b;
    return eAVSucceded;
  }
  
  /**
   * Read rgba values and store it in single variables a,r,g,b
   */
  inline AVResult    get( u_int8_t& r, u_int8_t& g, u_int8_t& b, u_int8_t& a ) const
  {
    a = (m_color >> 24) & 0xff; r = (m_color >> 16) & 0xff;
    g = (m_color >>  8) & 0xff; b = (m_color      ) & 0xff;

    return eAVSucceded;
  }
  
  /**
   * Compare current color with specified one.
   * Return true if colors are equals, false otherwise.
   */
  inline bool          compare( const CAVColor& cr ) const
  { return ((m_color&cr.m_color) == m_color); }
  
 
private:

  u_int32_t   m_color;
};

}

#endif // AVCOLOR_H
