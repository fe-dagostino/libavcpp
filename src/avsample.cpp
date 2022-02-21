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


#include "avsample.h"
#include <memory.h>

namespace libavcpp
{

CAVSample::CAVSample()
 : m_eFormat( AV_SAMPLE_FMT_NONE ), m_pFrame( NULL ),
   m_iChannels( -1 ), m_iSamples( -1 ), m_iAlign( -1 )
{
}

CAVSample::~CAVSample()
{
  if ( m_pFrame != NULL )
  {
    av_frame_free( &m_pFrame );
    m_pFrame = NULL;
  }
}

AVResult   CAVSample::init( 
			    const AVFrame*        pFrame, 
			    const AVCodecContext* pAVCodecCtx
		          )
{
  int  iResult = 0;  
  
  m_iChannels  = pAVCodecCtx->channels;
  m_iSamples   = pAVCodecCtx->sample_rate;
  m_eFormat    = pAVCodecCtx->sample_fmt;
  m_iAlign     = (pAVCodecCtx->block_align==0)?1:pAVCodecCtx->block_align;
  
  if ( pFrame != NULL )
  {
    m_pFrame = av_frame_alloc();
    if ( m_pFrame == NULL ) 
      return eAVNotEnoughMemory;
    
    m_pFrame->format         = pFrame->format;
    m_pFrame->nb_samples     = pFrame->nb_samples;
    m_pFrame->sample_rate    = pFrame->sample_rate;
    m_pFrame->channel_layout = pFrame->channel_layout;
    
    if (m_pFrame->nb_samples) 
    {
      if ( av_frame_get_buffer( m_pFrame, m_iAlign ) != 0 )
        return eAVNotEnoughMemory;
    }
    
    av_frame_copy( m_pFrame, pFrame );    
  }
  
  return eAVSucceded;
}

//getBuffer()

}

