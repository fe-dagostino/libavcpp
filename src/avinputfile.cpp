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


#include "../include/avinputfile.h"

extern "C"
{
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <FMutexCtrl.h>
}

namespace libavcpp
{

struct AVRawFrame
{
  AVRawFrame()
  {
    // Initialize AudioVideo packet with default values
    av_init_packet( &m_avPacket ); 
  }
  
  ~AVRawFrame()
  {
    // Release Packet allocated bt av_read_frame
    av_free_packet( &m_avPacket );
  }
  
  operator AVPacket*()
  { return &m_avPacket; }

  AVPacket		m_avPacket;
};
  
CAVInputFile::CAVInputFile()
 : CAVFile(), 
   m_uiAVFlags( AV_SET_BEST_VIDEO_CODEC ),
   m_pAVFormatContext( NULL ),
   m_bReachedEOF( false ),
   m_pTopRawFrame( NULL ),
   m_iBAS( -1 ), m_iBVS ( -1 ),
   m_pOptBAS( NULL ), m_pOptBVS( NULL ),
   m_pAudioCodec( NULL ),
   m_pVideoCodec( NULL )
{

}

CAVInputFile::~CAVInputFile()
{
   close();
}

AVResult CAVInputFile::open( const char* pFilename, unsigned int uiAVFlags )
{  
  FMutexCtrl     mtxCtrl( getMutex() );
  int            iResult      = 0;
  m_uiAVFlags   = uiAVFlags;
  m_bReachedEOF = false;

  // Open input file or stream
  iResult = avformat_open_input( &m_pAVFormatContext, pFilename, NULL, NULL );
  CHECK_AVRESULT( iResult )

  iResult = avformat_find_stream_info( m_pAVFormatContext, NULL );
  CHECK_AVRESULT( iResult )
  
  if ( m_uiAVFlags & AV_SET_BEST_AUDIO_CODEC )
  {
    iResult = av_find_best_stream( m_pAVFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &m_pAudioCodec, 0 );
    
    // Audio stream could be absent
    if ( iResult >= 0 )
    {
      m_iBAS = iResult;

      iResult = avcodec_open2( m_pAVFormatContext->streams[m_iBAS]->codec, m_pAudioCodec, &m_pOptBAS );
      CHECK_AVRESULT( iResult )
    }
  }

  if ( m_uiAVFlags & AV_SET_BEST_VIDEO_CODEC )
  {
    iResult = av_find_best_stream( m_pAVFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, &m_pVideoCodec, 0 );

    // Video stream could be absent
    if ( iResult >= 0 )
    {
      m_iBVS  = iResult;

      iResult = avcodec_open2( m_pAVFormatContext->streams[m_iBVS]->codec, m_pVideoCodec, &m_pOptBVS );
      CHECK_AVRESULT( iResult )    
    }
  }

  return eAVSucceded;
}

AVResult CAVInputFile::read( AVStream*& pAVStream, AVPacket*& pAVPacket, bool bBuffering )
{
  int iResult = 0;

  if ( m_pAVFormatContext == NULL )
    return eAVInvalidSequence;

  if ( m_bReachedEOF == false )
  {
    // Memory allocation for AudioVideo buffer item.
    // Allocation provide also to initialize one single AVPacket 
    AVRawFrame* pRawFrame = new AVRawFrame();
    if ( pRawFrame == NULL )
      return eAVNotEnoughMemory;
    
    // Read first available packet from file/stream
    iResult = av_read_frame( m_pAVFormatContext, *pRawFrame );
    if ( iResult == AVERROR_EOF )
    {
      m_bReachedEOF = true;
      
      delete pRawFrame;
      pRawFrame = NULL;
    }
    else
    {
      CHECK_AVRESULT( iResult )

      iResult = av_dup_packet( *pRawFrame );
      if ( iResult < 0 )
      {
        delete pRawFrame;
	pRawFrame = NULL;
	
	CHECK_AVRESULT( iResult )
      }
      
      // Enqueue new compressed frame
      m_queueInputFrames.push( pRawFrame );
    }
  }
  else
  {
    // In case the end of file has been reached, there is no reason to 
    // keep all buffered items, so on each call to Read() one item can be
    // released.
    if ( m_pTopRawFrame != NULL )
    {
      delete m_pTopRawFrame;
      m_pTopRawFrame = NULL;
    }
  }
  
  // Return until specified size has been reached.
  if (
      ( bBuffering    ==  true ) &&
      ( m_bReachedEOF == false )
     )
  {
    return eAVBuffering;
  } 

  // If buffer queue is empty and end of file has been reached
  // reading must be terminated.
  if ( 
	( m_bReachedEOF             == true ) &&
	( m_queueInputFrames.size() == 0    )
     )
  {
    return eAVEndOfFile;
  }
  
  if ( m_pTopRawFrame != NULL )
  {
    delete m_pTopRawFrame;
    m_pTopRawFrame = NULL;
  }
  
  // Extract first enqueued item.
  m_pTopRawFrame = m_queueInputFrames.front();
  m_queueInputFrames.pop();

  pAVStream = m_pAVFormatContext->streams[ m_pTopRawFrame->m_avPacket.stream_index ];

  // pAVPacket will be valid until next call to read().
  pAVPacket = *m_pTopRawFrame;
  
  return eAVSucceded;
}

AVResult CAVInputFile::close()
{
  FMutexCtrl  mtxCtrl( getMutex() );
  int         iResult = 0;
  
  if ( m_pAVFormatContext == NULL )
  {
    return eAVInvalidSequence;
  }

  // Release audio codec
  if ( m_uiAVFlags & AV_SET_BEST_AUDIO_CODEC )
  {
    if ( m_iBAS >= 0 )
    {
      iResult = avcodec_close( m_pAVFormatContext->streams[m_iBAS]->codec );
      CHECK_AVRESULT( iResult )   
    }
   
    // Set to initial value.
    m_pAudioCodec = NULL;
  }
  
  // Release video codec
  if ( m_uiAVFlags & AV_SET_BEST_VIDEO_CODEC )
  {
    if ( m_iBVS >= 0 )
    {
      iResult = avcodec_close( m_pAVFormatContext->streams[m_iBVS]->codec );
      CHECK_AVRESULT( iResult )   
    }
    
    // Set to initial value.
    m_pVideoCodec = NULL;    
  }  

  avformat_free_context(m_pAVFormatContext);
  // Set to initial value.
  m_pAVFormatContext = NULL;

  // Remove all buffered items.
  while ( !m_queueInputFrames.empty() )
  {
      AVRawFrame* _pFrame = m_queueInputFrames.front();
      m_queueInputFrames.pop();
      
      delete _pFrame;
  }

  m_iBAS = -1;
  m_iBVS = -1;
  
  return eAVSucceded;
}

bool CAVInputFile::isOpened() const
{
  return ( m_pAVFormatContext != NULL );
}

AVFormatContext* CAVInputFile::getAVFormatContext() const
{
  return m_pAVFormatContext;
}

AVCodec*         CAVInputFile::getAudioCodec() const
{
  return m_pAudioCodec;
}

AVCodec*         CAVInputFile::getVideoCodec() const
{
  return m_pVideoCodec;
}

}//namespace libavcpp

