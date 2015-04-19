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

#ifndef AVPOINT_H
#define AVPOINT_H

namespace libavcpp
{

/**
 * A simple point implementation.
 */
class CAVPoint
{
public:
  /***/
  CAVPoint();
  /***/
  CAVPoint( const CAVPoint& rPos );
  /***/
  CAVPoint( int x, int y );
  /***/
  virtual ~CAVPoint();

  /***/
  inline int  getX() const
  { return _x; }
  /***/
  inline int  getY() const
  { return _y; }

  /***/
  inline void set( const CAVPoint& rPos )
  { _x = rPos._x; _y = rPos._y; }

  /***/
  inline void set( int x, int y )
  { _x = x; _y = y; }

  /***/
  inline void add( int x, int y )
  { _x += x; _y += y; }

  /***/
  inline void sub( int x, int y )
  { _x -= x; _y -= y; }
  
private:  
  int _x, _y;
};

}

#endif // AVPOINT_H