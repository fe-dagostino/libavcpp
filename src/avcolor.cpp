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


#include "../include/avcolor.h"

namespace libavcpp
{
  
CAVColor::CAVColor()
{
  set( 0xFF, 0xFF, 0xFF, 0xFF );
}

CAVColor::CAVColor( const CAVColor& cr )
{
  m_color = cr.m_color;
}

CAVColor::CAVColor( u_int8_t r, u_int8_t g, u_int8_t b, u_int8_t a )
{
  set( r, g, b, a );
}

CAVColor::~CAVColor()
{
}

}//namespace libavcpp

