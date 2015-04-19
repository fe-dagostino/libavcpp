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


#include "../include/avsample.h"


namespace libavcpp
{

CAVSample::CAVSample()
 : m_eFormat( AV_SAMPLE_FMT_NONE )
{
  for ( register int ndx=0; ndx < AV_NUM_DATA_POINTERS; ndx++ )
  {
    m_pAudioData[ndx] = NULL;
    m_iLineSize[ndx]  = 0;
  }
}

CAVSample::~CAVSample()
{
  for ( register int ndx=0; ndx < AV_NUM_DATA_POINTERS; ndx++ )
  {  
    if ( m_pAudioData[ndx] != NULL )
    {
      av_free(m_pAudioData[ndx]);
      m_pAudioData[ndx] = NULL;
    }
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
  
  for ( register int ndx=0; ndx < AV_NUM_DATA_POINTERS; ndx++ )
  {
    if ( pFrame->linesize[ndx] > 0 )
    {
      m_pAudioData[ndx] = (uint8_t *)av_realloc( m_pAudioData[ndx], pFrame->linesize[ndx] );
      m_iLineSize[ndx]  = pFrame->linesize[ndx];
      
      memcpy( m_pAudioData[ndx], pFrame->data[ndx], m_iLineSize[ndx] );
    }
    else
    {
      if ( m_pAudioData[ndx] != NULL )
	av_free( m_pAudioData[ndx] );
      
      m_pAudioData[ndx] = NULL;
      m_iLineSize[ndx]  = 0;
    }
  }
  
  return eAVSucceded;
}

//getBuffer()

}

