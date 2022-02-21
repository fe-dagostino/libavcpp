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


#include "avencoder.h"
#include <iostream>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

namespace libavcpp
{

CAVEncoder::CAVEncoder( )
   : m_pAVOutputFile( NULL )
{
  
}

CAVEncoder::~CAVEncoder()
{
  // Check if the encoder is open and close it.
  if ( isOpened() )
  {
    close();
  }
  
  if ( m_pAVOutputFile != NULL )
  {
    delete m_pAVOutputFile;
    m_pAVOutputFile = NULL;
  }
}

AVResult CAVEncoder::open(
			    const char* pFilename,
			    unsigned int uiAVFlags,
			    int dstW, int dstH,
			    AVPixelFormat  dstPixFtm,
			    int dstFps, int dstGOP,
			    int dstBitRate,
			    AVCodecID   dstVideoCodec,
			    int dstVideoProfile
		       )
{
  AVResult eRetVal = eAVSucceded;
  
  CAVOutputFile::CAVStream* pStreams = new CAVOutputFile::CAVStream[3]();
  int                       iStream  = 0;
  
  if ( uiAVFlags & AV_ENCODE_VIDEO_STREAM )
    pStreams[iStream++] = CAVOutputFile::CAVStream( AVMEDIA_TYPE_VIDEO, dstVideoCodec, dstVideoProfile );
    
  if ( uiAVFlags & AV_ENCODE_AUDIO_STREAM )
    pStreams[iStream++] = CAVOutputFile::CAVStream( AVMEDIA_TYPE_AUDIO, AV_CODEC_ID_MP3, 0 );
     
  m_pAVOutputFile = new CAVOutputFile( dstW, dstH, dstPixFtm, dstFps, dstGOP, dstBitRate, pStreams );
  if ( m_pAVOutputFile == NULL )
    return eAVNotEnoughMemory;
  
  // Open output file. 
  eRetVal = m_pAVOutputFile->open( pFilename, uiAVFlags );
  if ( eRetVal != eAVSucceded )
      return eRetVal;
  
  if ( uiAVFlags & AV_ENCODE_VIDEO_STREAM )
  {
  }

  if ( uiAVFlags & AV_ENCODE_AUDIO_STREAM )
  {
  }

  return eRetVal;  
}

int CAVEncoder::getVideoWidth() const
{
  if ( m_pAVOutputFile == NULL )
    return -1;
  
  AVCodecContext* pCodecContext = m_pAVOutputFile->getAVCodecContext( AVMEDIA_TYPE_VIDEO );
  if ( pCodecContext == NULL )
    return -1;
  
  return pCodecContext->width;
}

int CAVEncoder::getVideoHeight() const
{
  if ( m_pAVOutputFile == NULL )
    return -1;
  
  AVCodecContext* pCodecContext = m_pAVOutputFile->getAVCodecContext( AVMEDIA_TYPE_VIDEO );
  if ( pCodecContext == NULL )
    return -1;
  
  return pCodecContext->height;
}

AVResult CAVEncoder::write( const CAVImage* pAVFrame, unsigned int uiFlags )
{
  AVCodecContext* pCodecContext = m_pAVOutputFile->getAVCodecContext( AVMEDIA_TYPE_VIDEO );
  if ( pCodecContext == NULL )
    return eAVStreamNotFound;
  
  if ( (pAVFrame->getWidth()  != pCodecContext->width  ) ||
       (pAVFrame->getHeight() != pCodecContext->height )    )
    return eAVInvalidParameters;
  
  AVResult  eResult     = eAVSucceded;
  AVPacket  avPkt;      
  int       iGotPacket  = 0;
  
  // Initialize av packet
  av_init_packet( &avPkt ); 
  
  if ( avcodec_encode_video2( pCodecContext, &avPkt, pAVFrame->getFrame(), &iGotPacket ) == 0 )
  {
    if ( iGotPacket == 1 )
    {
      // Write new video packet.
      eResult = m_pAVOutputFile->write( AVMEDIA_TYPE_VIDEO, avPkt, uiFlags );
      
      av_free_packet( &avPkt ); 
    } // if ( iGotPacket == 1 )
  }
      
  return eResult;
}

AVResult CAVEncoder::write( const CAVSample* pAVFrame, unsigned int uiFlags )
{
  AVCodecContext* pCodecContext = m_pAVOutputFile->getAVCodecContext( AVMEDIA_TYPE_VIDEO );
  if ( pCodecContext == NULL )
    return eAVStreamNotFound;
  
  AVResult  eResult     = eAVSucceded;
  AVPacket  avPkt;
  int       iGotPacket  = 0;
  
  // Initialize av packet
  av_init_packet( &avPkt ); 

  if ( avcodec_encode_audio2( pCodecContext, &avPkt, pAVFrame->getFrame(), &iGotPacket ) == 0 )
  {
    if ( iGotPacket == 1 )
    {
      // Write new audio packet.
      eResult = m_pAVOutputFile->write( AVMEDIA_TYPE_AUDIO, avPkt, uiFlags );

      av_free_packet( &avPkt ); 
    } // if ( iGotPacket == 1 )
  }
      
  return eResult;
}

AVResult CAVEncoder::write( const CAVFrame* pAVFrame, unsigned int uiFlags )
{
  if  (pAVFrame == NULL )
    return eAVInvalidParameters;
    
  switch ( pAVFrame->getMediaType() )
  {
    case AVMEDIA_TYPE_VIDEO:
    {
      return write( pAVFrame->getImage(), uiFlags );
    }; break;
    
    case AVMEDIA_TYPE_AUDIO:
    {
      return write( pAVFrame->getSample(), uiFlags );
    }; break;
  }

  return eAVSucceded;
}

AVResult CAVEncoder::flush( unsigned int uiFlags )
{
  return m_pAVOutputFile->flush( uiFlags );
}

AVResult CAVEncoder::close()
{
  return m_pAVOutputFile->close();
}

bool  CAVEncoder::isOpened() const
{
  if ( m_pAVOutputFile == NULL )
    return false;
  
  return m_pAVOutputFile->isOpened();
}

}//namespace libavcpp

