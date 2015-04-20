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


#ifndef AVAPPLICATION_H
#define AVAPPLICATION_H

#include "avcommon.h"

namespace libavcpp
{

class CAVApplication
{
public:
  
 
  /**
   * Inialize globals requirements.
   */
  static AVResult initLibAVCPP();
  /**
   * Release globals allocaitons.
   */
  static AVResult deinitLibAVCPP();
  
  
};
  
}//namespace libavcpp

#endif //AVAPPLICATION_H