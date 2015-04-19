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


#ifndef AVFRAME_H
#define AVFRAME_H

#include "avcommon.h"
#include "avimage.h"
#include "avsample.h"

namespace libavcpp
{
  

/**
*/
class CAVFrame
{
public:
  /***/
  CAVFrame( CAVImage*  pAVImage  );
  /***/
  CAVFrame( CAVSample* pAVSample );
  /***/
  ~CAVFrame();

  /***/
  inline AVMediaType getMediaType() const
  { return m_avMediaType; }
  
  /***/
  inline CAVImage*   getImage() const
  { return m_pAVImage;  }
  
  /***/
  inline CAVSample*  getSample() const
  { return m_pAVSample; }
  
  /**
   * Detach internal pointer in order to avoid 
   * releasing on destructor.
   */
  void               detach();
  
private:
  AVMediaType m_avMediaType;
  CAVImage*   m_pAVImage;
  CAVSample*  m_pAVSample;
};

}


#endif // AVFRAME_H
