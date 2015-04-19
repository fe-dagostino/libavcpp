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


#include "../include/avframe.h"


namespace libavcpp
{

CAVFrame::CAVFrame( CAVImage*  pAVImage  )
 : m_avMediaType( AVMEDIA_TYPE_VIDEO ), m_pAVImage( pAVImage ), m_pAVSample( NULL )
{
  
}
  
CAVFrame::CAVFrame( CAVSample* pAVSample )
 : m_avMediaType( AVMEDIA_TYPE_AUDIO ), m_pAVImage( NULL ), m_pAVSample( pAVSample )
{
  
}
  
CAVFrame::~CAVFrame()
{
  if ( m_pAVImage != NULL )
  {
    delete m_pAVImage;
    m_pAVImage = NULL;
  }  
  
  if ( m_pAVSample != NULL )
  {
    delete m_pAVSample;
    m_pAVSample = NULL;
  }  
}
  
void   CAVFrame::detach()
{
  m_avMediaType = AVMEDIA_TYPE_UNKNOWN;
  m_pAVImage    = NULL;
  m_pAVSample   = NULL;
}
  
}

