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


#include "../include/avencoder.h"
#include <iostream>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/fifo.h>
}

namespace libavcpp
{

CAVEncoder::CAVEncoder( )
   : m_pAVOutputFile( NULL ),
     m_pVideoBuffer( NULL ),
     m_iVideoBufSize( -1 ),
     m_pSrcAudioBuffer( NULL ),
     m_pEncAudioBuffer( NULL ),
     m_iAudioBufSize( -1 ),
     m_pFifoBuffer( NULL )
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
			    PixelFormat  dstPixFtm,
			    int dstFps, int dstGOP,
			    int dstBitRate,
			    CodecID   dstVideoCodec,
			    int dstVideoProfile
		       )
{
  AVResult eRetVal = eAVSucceded;
  
  CAVOutputFile::CAVStream* pStreams = new CAVOutputFile::CAVStream[3]();
  int                       iStream  = 0;
  
  
  if ( uiAVFlags & AV_ENCODE_VIDEO_STREAM )
    pStreams[iStream++] = CAVOutputFile::CAVStream( AVMEDIA_TYPE_VIDEO, dstVideoCodec, dstVideoProfile );
    
  if ( uiAVFlags & AV_ENCODE_AUDIO_STREAM )
    pStreams[iStream++] = CAVOutputFile::CAVStream( AVMEDIA_TYPE_AUDIO, CODEC_ID_MP3, 0 );
     
  m_pAVOutputFile = new CAVOutputFile( dstW, dstH, dstPixFtm, dstFps, dstGOP, dstBitRate, pStreams );
  if ( m_pAVOutputFile == NULL )
    return eAVNotEnoughMemory;
  
  // Open output file. 
  eRetVal = m_pAVOutputFile->open( pFilename, uiAVFlags );
  if ( eRetVal != eAVSucceded )
      return eRetVal;
  
  if ( uiAVFlags & AV_ENCODE_VIDEO_STREAM )
  {
    // Determine required buffer size and allocate buffer
    m_iVideoBufSize = avpicture_get_size(
					  m_pAVOutputFile->getAVCodecContext(AVMEDIA_TYPE_VIDEO)->pix_fmt,
					  m_pAVOutputFile->getAVCodecContext(AVMEDIA_TYPE_VIDEO)->width,
					  m_pAVOutputFile->getAVCodecContext(AVMEDIA_TYPE_VIDEO)->height
					);
					  
    m_pVideoBuffer = (uint8_t*)av_malloc( m_iVideoBufSize );
    if ( m_pVideoBuffer == NULL )
    {
      //@todo
    }  
  }

  if ( uiAVFlags & AV_ENCODE_AUDIO_STREAM )
  {
    //Audio Buffer
    m_iAudioBufSize   = AVCODEC_MAX_AUDIO_FRAME_SIZE;
    m_iSrcBufferPos   = 0;
    m_pSrcAudioBuffer = (uint8_t*)av_malloc( m_iAudioBufSize );
    m_pEncAudioBuffer = (uint8_t*)av_malloc( m_iAudioBufSize );
    if ( 
         ( m_pSrcAudioBuffer == NULL ) ||
         ( m_pEncAudioBuffer == NULL ) 
       )
    {
      //@todo
    }
  }

  m_pFifoBuffer = av_fifo_alloc(1024);
  if ( m_pFifoBuffer == NULL )
  {
    //@todo
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
  int       iPacketSize = avcodec_encode_video( pCodecContext, m_pVideoBuffer, m_iVideoBufSize, pAVFrame->getFrame() );
  AVPacket  avPkt;      
  av_init_packet( &avPkt ); 

  if ( iPacketSize > 0 )
  {
    if(pCodecContext->coded_frame->key_frame)
    {
      avPkt.flags |= AV_PKT_FLAG_KEY;
    }

    // m_pAVPacket->pts          updated by CAVOutputFile  
    // m_pAVPacket->stream_index updated by CAVOutputFile
    avPkt.data= m_pVideoBuffer;
    avPkt.size= iPacketSize;     

    // Write new video packet.
    eResult = m_pAVOutputFile->write( AVMEDIA_TYPE_VIDEO, avPkt, uiFlags );
    
    av_free_packet( &avPkt ); 
  }//if ( iPacketSize > 0 )
      
  return eResult;
}

AVResult CAVEncoder::write( const CAVSample* pAVFrame, unsigned int uiFlags )
{
  AVCodecContext* pCodecContext = m_pAVOutputFile->getAVCodecContext( AVMEDIA_TYPE_VIDEO );
  if ( pCodecContext == NULL )
    return eAVStreamNotFound;
  
  AVResult  eResult     = eAVSucceded;

  // output raw samples
  if (av_fifo_realloc2( m_pFifoBuffer, av_fifo_size(m_pFifoBuffer) + pAVFrame->getSize(0) ) < 0)
  {
    //@todo
  }
  av_fifo_generic_write(m_pFifoBuffer, pAVFrame->getData(0), pAVFrame->getSize(0), NULL);

  int iEncFrameSize = pCodecContext->frame_size * pCodecContext->channels *  av_get_bytes_per_sample(pCodecContext->sample_fmt);
      
  while ( av_fifo_size(m_pFifoBuffer) >= iEncFrameSize )
  {
    // Read just the amount of bytes needed by the encoder 
    av_fifo_generic_read(m_pFifoBuffer, m_pSrcAudioBuffer, iEncFrameSize, NULL );
    
    int      iEncPacketSize = avcodec_encode_audio( pCodecContext, m_pEncAudioBuffer, m_iAudioBufSize, (short*)m_pSrcAudioBuffer );
    AVPacket avPkt;
    
    av_init_packet( &avPkt ); 

    // m_pAVPacket->pts          updated by CAVOutputFile  
    // m_pAVPacket->stream_index updated by CAVOutputFile
    avPkt.flags |= AV_PKT_FLAG_KEY;
    avPkt.data   = m_pEncAudioBuffer;
    avPkt.size   = iEncPacketSize;     

    // Write new audio packet.
    eResult = m_pAVOutputFile->write( AVMEDIA_TYPE_AUDIO, avPkt, uiFlags );

    av_free_packet( &avPkt ); 
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
  if ( m_pVideoBuffer == NULL )
    return  eAVInvalidSequence;

  return m_pAVOutputFile->flush( uiFlags );
}

AVResult CAVEncoder::close()
{
  if ( m_pVideoBuffer != NULL )
  {
    av_free( m_pVideoBuffer ); 
    m_pVideoBuffer  = NULL;
    m_iVideoBufSize = -1;
  }
  
  if ( m_pEncAudioBuffer != NULL )
  {
    av_free( m_pEncAudioBuffer );
    m_pEncAudioBuffer  = NULL;
    m_iAudioBufSize = -1;
  }

  if ( m_pSrcAudioBuffer != NULL )
  {
    av_free( m_pSrcAudioBuffer );
    m_pSrcAudioBuffer  = NULL;
    m_iSrcBufferPos    = 0;
  }

  if ( m_pFifoBuffer != NULL )
  {
    av_fifo_free(m_pFifoBuffer);
    m_pFifoBuffer = NULL;
  }
  
  return m_pAVOutputFile->close();
}

bool  CAVEncoder::isOpened() const
{
  if ( m_pAVOutputFile == NULL )
    return false;
  
  return m_pAVOutputFile->isOpened();
}

}//namespace libavcpp

