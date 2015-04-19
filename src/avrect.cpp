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


#include "../include/avrect.h"

namespace libavcpp
{

CAVRect::CAVRect()
  : _x(0), _y(0), _w(0), _h(0)
{
}

CAVRect::CAVRect( int x, int y, int w, int h )
  : _x(x), _y(y), _w(w), _h(h)
{
}

CAVRect::~CAVRect()
{
}

bool CAVRect::isEmpty() const
{  return (_w==_h==0); }

}
