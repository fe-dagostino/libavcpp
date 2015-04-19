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


#include "../include/avdecoder.h"
#include "../include/avimage.h"


#include "FMutexCtrl.h"

USING_NAMESPACE_FED

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/avfiltergraph.h>
}

namespace libavcpp
{

CAVDecoder::CAVDecoder()
  : m_pEvents( NULL ),
  m_bAutoRelease( true ),
  m_dStartTime( 0.0 ),
  m_dBufferTime( 0.0 ),
  m_pAVFilterGraph( NULL )
{

}

CAVDecoder::~CAVDecoder()
{
  // Check if the Decoder is open and close it.
  if ( isOpened() )
  {
    close();
  }
  
  if (
      ( m_bAutoRelease == true ) &&
      ( m_pEvents      != NULL )
  )
  {
    delete m_pEvents;
    m_pEvents = NULL;
  }
  
  if ( m_pAVFilterGraph != NULL )
  {
    delete m_pAVFilterGraph;
    m_pAVFilterGraph = NULL;
  }
}

AVResult CAVDecoder::setDecoderEvents( IAVDecoderEvents* pEvents, bool bAutoRelease )
{
  AVResult eRetVal = eAVSucceded;
  m_pEvents      = pEvents;
  m_bAutoRelease = bAutoRelease;
  return eRetVal;
}

AVResult CAVDecoder::getDecoderEvents( IAVDecoderEvents*& pEvents )
{
  AVResult eRetVal = eAVSucceded;
  
  pEvents = m_pEvents;
 
  return eRetVal;
} 

AVResult CAVDecoder::open( const char* pFilename, double dBufferTime, unsigned int uiAVFlags )
{
  AVResult eRetVal = eAVSucceded;
  
  // Open input file or stream
  eRetVal = m_avInputFile.open( pFilename, uiAVFlags );
  if ( eRetVal != eAVSucceded )
    return eRetVal;

  m_dStartTime  = getTime();
  m_dBufferTime = dBufferTime;
  
  return eRetVal;
}

AVResult CAVDecoder::read( unsigned int wFlags )
{  
  AVResult         eRetVal     = eAVSucceded;
  AVCodecContext*  pAVCodecCtx = NULL;
  AVStream*        pAVStream   = NULL;
  AVPacket*        pAVPacket   = NULL;
  AVFrame*         pAVFrame    = avcodec_alloc_frame();
  bool             bExit       = false;
  
  while ( bExit == false )
  {
    bool bBuffering  = (( getTime() - m_dStartTime ) < m_dBufferTime );
  
    eRetVal = m_avInputFile.read( pAVStream, pAVPacket, bBuffering );
    if ( eRetVal == eAVBuffering )
    {
      if ( wFlags & AVD_EXIT_ON_BUFFERING )
	break;
      else
	continue;
    }
    if ( eRetVal != eAVSucceded )
    {
      bExit = true;
      continue;
    }
    
    pAVCodecCtx = pAVStream->codec;
   
    avcodec_get_frame_defaults( pAVFrame );
    
    switch ( pAVCodecCtx->codec_type )
    {
      case AVMEDIA_TYPE_UNKNOWN:
      {
	
      }; break;
      case AVMEDIA_TYPE_VIDEO:
      {
	if (!( m_avInputFile.getFlags() & AV_SET_BEST_VIDEO_CODEC ))
	  break;
	
	if ( m_pEvents != NULL )
	{
	  m_pEvents->OnVideoPacket( pAVPacket, pAVStream, pAVCodecCtx );
	}	
	
	int iGotPitcure = 0;
	avcodec_decode_video2( pAVCodecCtx, pAVFrame, &iGotPitcure, pAVPacket );

    if ( pAVFrame->pts == AV_NOPTS_VALUE )
    {
        // Try grabbing DTS from packet
        if ((pAVPacket->dts == AV_NOPTS_VALUE) && (pAVFrame->reordered_opaque != AV_NOPTS_VALUE))
        {
          pAVFrame->pts = pAVFrame->reordered_opaque;
        }
        else if (pAVPacket->dts != AV_NOPTS_VALUE)
        {
          pAVFrame->pts = pAVPacket->dts;
        }
        else
        {
          pAVFrame->pts = 0.0;
        }
    }

	if (iGotPitcure)
	{
	  if ( m_pEvents != NULL )
	  {    
	    double dTime =  pAVFrame->pts * av_q2d(pAVCodecCtx->time_base);

	    // Raise event just if the frame is a key frame
	    if ( pAVFrame->key_frame == 1 )
	      m_pEvents->OnVideoKeyFrame( pAVFrame, pAVStream, pAVCodecCtx, dTime );
	    
	    if ( m_pEvents->OnVideoFrame( pAVFrame, pAVStream, pAVCodecCtx, dTime ) == false )
	    {
	      bExit = true;
	    }
	    
	    FMutexCtrl  mtxCtrl( m_mtxFilterGraph );
	    if ( m_pAVFilterGraph != NULL )
	    {
	      // Check if filtering has been initialized or not
	      if ( !m_pAVFilterGraph->isValid() )
	      {
		m_pAVFilterGraph->init( pAVCodecCtx );
	      }
	      
	      bool                bMore              = true;
	      AVFilterBufferRef*  pAVFilterBufferRef = NULL;
	      
	      if ( m_pAVFilterGraph->push( pAVFrame, pAVCodecCtx ) ==  eAVSucceded )
	      {
                while ( bMore )
                {
                  m_pAVFilterGraph->pop( bMore, pAVFilterBufferRef );
		  
                  if ( m_pEvents->OnFilteredVideoFrame( pAVFilterBufferRef, pAVStream, pAVCodecCtx, dTime ) == false )
                  {
                    bExit = true;
                  }
		  
                  m_pAVFilterGraph->unRef( pAVFilterBufferRef );
                }
	      }
	      else
	      {
            // @todo raise error
	      }
	    } // if ( m_pAVFilterGraph != NULL )
	  }
	
	  if ( wFlags & AVD_EXIT_ON_VIDEO_KEY_FRAME  )
	  {
	    // Check if current frame is a key frame.
	    if ( pAVFrame->key_frame == 1 )
	      bExit = true;
	  }
	  else if ( wFlags & AVD_EXIT_ON_NEXT_VIDEO_FRAME )
	    bExit = true;
	  
	}
      }; break;
      case AVMEDIA_TYPE_AUDIO:
      {
	if (!( m_avInputFile.getFlags() & AV_SET_BEST_AUDIO_CODEC ))
	  break;
	
	if ( m_pEvents != NULL )
	{
	  m_pEvents->OnAudioPacket( pAVPacket, pAVStream, pAVCodecCtx );
	}	
	
	int iGotFrame = 0;
	avcodec_decode_audio4( pAVCodecCtx, pAVFrame, &iGotFrame, pAVPacket );
	if (iGotFrame)
	{
	  if ( m_pEvents != NULL )
	  {    
	    double dTime =  pAVFrame->pts * av_q2d(pAVCodecCtx->time_base);

	    if ( m_pEvents->OnAudioFrame( pAVFrame, pAVStream, pAVCodecCtx, dTime ) == false )
	    {
	      bExit = true;
	    }
	  }
	
	  if ( wFlags & AVD_EXIT_ON_NEXT_AUDIO_FRAME )
	    bExit = true;
	}
	
      }; break;
      case AVMEDIA_TYPE_DATA:
      {
	
      }; break;
      case AVMEDIA_TYPE_SUBTITLE:
      {
	
      }; break;
      case AVMEDIA_TYPE_ATTACHMENT:
      {
	
      }; break;
      case AVMEDIA_TYPE_NB:
      {
	
      }; break;
    }
  }
    
  av_free( pAVFrame );
  pAVFrame = NULL;
  
  return eRetVal;
}


AVResult CAVDecoder::close()
{
  m_dStartTime  = 0.0;
  m_dBufferTime = 0.0;
  
  return m_avInputFile.close();  
}

bool    CAVDecoder::isOpened() const
{
  return m_avInputFile.isOpened();
}

CAVFilterGraph*    CAVDecoder::setFilterGraph( CAVFilterGraph* pAVFilterGraph )
{ 
  FMutexCtrl  _mtxCtrl( m_mtxFilterGraph );
  
  CAVFilterGraph* _pRetVal = m_pAVFilterGraph;
  
  m_pAVFilterGraph = pAVFilterGraph; 
  
  return _pRetVal;
}

CAVFilterGraph*  CAVDecoder::getFilterGraph() const
{ 
  CAVFilterGraph* _pRetVal = NULL;
  
  m_mtxFilterGraph.EnterMutex();
    _pRetVal = m_pAVFilterGraph; 
  m_mtxFilterGraph.LeaveMutex();
  
  return _pRetVal;
}


double  CAVDecoder::getTime()
{
  return (av_gettime()/1000000.0);
}

}//namespace libavcpp

