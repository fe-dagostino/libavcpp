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


#ifndef AVSAMPLE_H
#define AVSAMPLE_H

#include "avcommon.h"

extern "C"
{ 
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
}

namespace libavcpp
{

class CAVSample
{
public:
  /***/
  CAVSample();
  /***/
  ~CAVSample();

  /**
   *
   */
  AVResult       init( 
			const AVFrame*        pFrame, 
			const AVCodecContext* pAVCodecCtx
		     );

  /**
   *  Return sample Format.
   */
  inline enum AVSampleFormat     getFormat() const
  { return m_eFormat;   }
  /**
   *  Return audio channels.
   */
  inline int                     getChannels() const
  { return m_iChannels; }
  
  /**
   * Return maximum number of buffers. 
   * This value must be used as upper limit for indexing
   * both getData and getSize
   */
  inline int                     getMaxBuffers() const
  { return AV_NUM_DATA_POINTERS; }
  
  /**
   */
  inline const AVFrame*          getFrame() const 
  { return m_pFrame; }
  
private:  
  AVFrame*                m_pFrame;
  enum AVSampleFormat     m_eFormat;
  int                     m_iChannels;
  int                     m_iSamples;
  int                     m_iAlign;
};

}


#endif // AVSAMPLE_H
