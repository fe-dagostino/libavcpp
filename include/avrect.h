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


#ifndef AVRECT_H
#define AVRECT_H

#include "avpoint.h"

namespace libavcpp
{

/**
 * A simple rect implementation.
 */
class CAVRect
{
public:
  /***/
  CAVRect();
  /***/
  CAVRect( int x, int y, int w, int h );
  /***/
  virtual ~CAVRect();

  /***/
  bool isEmpty() const;
  
  /***/
  inline int getX() const
  { return _x; }
  /***/
  inline int getY() const
  { return _y; }
  /***/
  inline int getWidth() const
  { return _w; }
  /***/
  inline int getHeight() const
  {  return _h; }
  
private:  
  int _x, _y;
  int _w, _h;
};

}

#endif // AVRECT_H
