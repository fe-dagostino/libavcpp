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


#include "../include/avoutputfile.h"
#include "../include/avimage.h"
#include "FMutexCtrl.h"
#include "LOGGING/FLogger.h"

extern "C"
{
#include <libavutil/avutil.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
}

USING_NAMESPACE_FED

namespace libavcpp
{

CAVOutputFile::CAVOutputFile( 
			      int         dstWidth,
			      int         dstHeight,
			      PixelFormat dstFormat,
			      int         dstFPS,
			      int         dstGOP,
			      int         dstBitRate,
			      CAVStream*  pDstStreams 
			    )
 : CAVFile(), 
   m_uiAVFlags( AV_SET_BEST_AUDIO_CODEC|AV_SET_BEST_VIDEO_CODEC ),
   m_pDstStreams( pDstStreams ),
   m_pAVFormatContext( NULL ),
   m_dstWidth( dstWidth ),
   m_dstHeight( dstHeight ),
   m_dstFormat( dstFormat ),
   m_dstFPS( dstFPS ),
   m_dstGOP( dstGOP ),
   m_dstBitRate( dstBitRate ),
   m_iBAS( -1 ), m_iBVS( -1 ), 
   m_optBAS( NULL ), m_optBVS( NULL ),
   m_pAudioCodec( NULL ),
   m_pVideoCodec( NULL ),
   m_lstOutputFiles( NULL )
{

}

CAVOutputFile::~CAVOutputFile()
{
  if ( m_pDstStreams != NULL )
  {
    delete [] m_pDstStreams;
    m_pDstStreams = NULL;
  }
  
  close();
}


AVResult CAVOutputFile::open( const char* pFilename, unsigned int uiAVFlags )
{ 
  FMutexCtrl  mtxCtrl( getMutex() );
  int         iResult = 0;
  
  if ( pFilename == NULL )
    return eAVInvalidParameters;

  // Check filename lengh; it will be less than 1024 that is limit
  // imposed from AVFormatContext.filename buffer.
  if ( strlen(pFilename) >= 1023 )
    return eAVTooLongFilename;
  
  // Alloc new context
  m_pAVFormatContext = avformat_alloc_context();
  if ( m_pAVFormatContext == NULL )
  {
    return eAVNotEnoughMemory;
  }
  
  // Check if specified destination stream is valid.
  if ( m_pDstStreams == NULL )
  {
    avformat_free_context(m_pAVFormatContext);
    // Set to initial value.
    m_pAVFormatContext = NULL;
  
    return eAVInvalidParameters;
  }
  
  /* auto detect the output format from the name. default is mpeg. */
  m_pAVFormatContext->oformat = av_guess_format( NULL, pFilename, NULL );
  if ( m_pAVFormatContext->oformat == NULL )
  {
    m_pAVFormatContext->oformat = av_guess_format("mpeg", NULL, NULL);
  }
  
  // check if output format has been found 
  if ( m_pAVFormatContext->oformat == NULL )
  {
    avformat_free_context(m_pAVFormatContext);
    // Set to initial value.
    m_pAVFormatContext = NULL;
      
    return eAVUnsupportedOutputFormat;
  }
  
  // Validity both for file name and content has been made at the beginning
  // of this method.
  sprintf( m_pAVFormatContext->filename, "%s", pFilename );

  // Set streams
  int iStreamIndex = 0;
  while( 
        (m_pDstStreams[iStreamIndex].getMediaType() != AVMEDIA_TYPE_UNKNOWN ) &&
	(m_pDstStreams[iStreamIndex].getCodecID()   != CODEC_ID_NONE        )
       )
  {
    if ( m_pDstStreams[iStreamIndex].getMediaType() == AVMEDIA_TYPE_VIDEO )
    {
      if ( uiAVFlags & AV_ENCODE_VIDEO_STREAM )
      {
	AVStream* pStream = add_video_stream( 
					      iStreamIndex, 
					      m_pDstStreams[iStreamIndex].getCodecID(),
					      m_pDstStreams[iStreamIndex].getProfile()
					    );
	if ( pStream != NULL )
	  m_iBVS = iStreamIndex;
      }
    }
    else
    if ( m_pDstStreams[iStreamIndex].getMediaType() == AVMEDIA_TYPE_AUDIO )
    {    
      if ( uiAVFlags & AV_ENCODE_AUDIO_STREAM )
      {
	AVStream* pStream = add_audio_stream( 
					      iStreamIndex, 
					      m_pDstStreams[iStreamIndex].getCodecID(),
					      m_pDstStreams[iStreamIndex].getProfile()
					    );
	if ( pStream != NULL )
	  m_iBAS = iStreamIndex;
      }
    }
    else
    {
      //unmanaged stream  
    }
    
    iStreamIndex++;
  }
  
  // Open video codec.
  if ( m_iBVS >= 0 )
  {
    AVCodecContext* pCodecContext = m_pAVFormatContext->streams[m_iBVS]->codec;
  
    if ( getVideoCodec() == NULL )
    {
      avformat_free_context(m_pAVFormatContext);
      // Set to initial value.
      m_pAVFormatContext = NULL;
 
      return eAVEncoderNotFound;
    }
        
    // open the codec 
    iResult = avcodec_open2( pCodecContext, m_pVideoCodec, &m_optBVS );
    CHECK_AVRESULT( iResult )
  }
  
  // Open Audio codec.
  if ( m_iBAS >= 0 )
  {
    AVCodecContext* pCodecContext = m_pAVFormatContext->streams[m_iBAS]->codec;

    if ( getAudioCodec() == NULL )
    {
      avformat_free_context(m_pAVFormatContext);
      // Set to initial value.
      m_pAVFormatContext = NULL;
 
      return eAVEncoderNotFound;
    }

    // open the codec 
    iResult = avcodec_open2( pCodecContext, getAudioCodec(), &m_optBAS );
    CHECK_AVRESULT( iResult )
  }

  // open the output file, if needed 
  if ( !(m_pAVFormatContext->oformat->flags & AVFMT_NOFILE) ) 
  {
    // Check using of dyn buffer
    if ( uiAVFlags & AV_USE_DYN_BUF_IO )
    {
      // Create, Open and appends file to Output File List
      if ( ofs_open( pFilename ) == false )
      {
	//@todo
      }
    }
    else
    {
      if (avio_open(&m_pAVFormatContext->pb, m_pAVFormatContext->filename, AVIO_FLAG_WRITE) < 0) 
      {
	fprintf(stderr, "Could not open '%s'\n", m_pAVFormatContext->filename);
	exit(1);
	//@todo
      }
    }
  }

  // Check using of dyn buffer
  if ( uiAVFlags & AV_USE_DYN_BUF_IO )
  {
    if ( avio_open_dyn_buf( &m_pAVFormatContext->pb ) != 0 )
    {
      //@todo error tracing
    }
  }
    
  // Initialize file with header
  iResult = avformat_write_header( m_pAVFormatContext, NULL );
  if ( iResult < 0 )
  {
    //@todo
  }

  // Check using of dyn buffer
  if ( uiAVFlags & AV_USE_DYN_BUF_IO )
  {
    uint8_t *pTmpBuf  = NULL;
    int      iBufSize = avio_close_dyn_buf( m_pAVFormatContext->pb, &pTmpBuf );
    if ( iBufSize > 0 )
    {
      if ( ofs_write( pTmpBuf, iBufSize ) == false )
      {
	//@todo error tracing
      }
    }
    
    av_free( pTmpBuf );
  }

  m_pAVFormatContext->max_delay = (int)(0.7 * AV_TIME_BASE);
  
  // Set control flags
  m_uiAVFlags = uiAVFlags;
  
  return eAVSucceded;
}

AVResult CAVOutputFile::write( enum AVMediaType avMediaType, AVPacket& rAVPacket,  unsigned int uiAVFlags )
{
  int  iResult      = 0;
  bool bInterleaved = false; 

  if ( m_pAVFormatContext == NULL )
    return eAVInvalidSequence;

  switch ( avMediaType )
  {
    case AVMEDIA_TYPE_VIDEO:
      bInterleaved = (uiAVFlags & AV_INTERLEAVED_VIDEO_WR);
      rAVPacket.stream_index = m_iBVS;
    break;
    case AVMEDIA_TYPE_AUDIO:
    {
      bInterleaved = (uiAVFlags & AV_INTERLEAVED_AUDIO_WR);
      rAVPacket.stream_index = m_iBAS;
    }; break;
    default:
      return eAVInvalidParameters;
  }
  
  if ( 
       (m_pAVFormatContext->streams[rAVPacket.stream_index]->codec->coded_frame) && 
       (m_pAVFormatContext->streams[rAVPacket.stream_index]->codec->coded_frame->pkt_pts != AV_NOPTS_VALUE )
     )
  {
    rAVPacket.pts       = av_rescale_q(
				  m_pAVFormatContext->streams[rAVPacket.stream_index]->codec->coded_frame->pts,
				  m_pAVFormatContext->streams[rAVPacket.stream_index]->codec->time_base,
				  m_pAVFormatContext->streams[rAVPacket.stream_index]->time_base
				);
  }
  
  // Check using of dyn buffer
  if ( m_uiAVFlags & AV_USE_DYN_BUF_IO )
  {
    if ( avio_open_dyn_buf( &m_pAVFormatContext->pb ) != 0 )
    {
      //@todo error tracing
    }
  }  
  
  AVPacket* _pAVPacket = &rAVPacket;
  
  if ( bInterleaved == true )
    iResult = av_interleaved_write_frame( m_pAVFormatContext, _pAVPacket );
  else
    iResult = av_write_frame( m_pAVFormatContext, _pAVPacket );
  CHECK_AVRESULT( iResult )
  
  // Check using of dyn buffer
  if ( m_uiAVFlags & AV_USE_DYN_BUF_IO )
  {
    uint8_t *pTmpBuf  = NULL;
    int      iBufSize = avio_close_dyn_buf( m_pAVFormatContext->pb, &pTmpBuf );
    if ( iBufSize > 0 )
    {
      if ( ofs_write( pTmpBuf, iBufSize ) == false )
      {
	//@todo error tracing
      }
    }
    
    av_free( pTmpBuf );
  }  
  
  return eAVSucceded;
}

AVResult CAVOutputFile::flush( unsigned int uiAVFlags )
{ 
  int  iResult      = 0;
  bool bInterleaved = false; 

  if ( m_pAVFormatContext == NULL )
    return eAVInvalidSequence;

  bInterleaved = (uiAVFlags & AV_INTERLEAVED_VIDEO_WR) || (uiAVFlags & AV_INTERLEAVED_AUDIO_WR);
  
  if ( bInterleaved == true )
    iResult = av_interleaved_write_frame( m_pAVFormatContext, NULL );
  else
    iResult = av_write_frame( m_pAVFormatContext, NULL );
  CHECK_AVRESULT( iResult )
  
  return eAVSucceded;
}

AVResult CAVOutputFile::close()
{
  FMutexCtrl  mtxCtrl( getMutex() );
  int         iResult = 0;
  
  if ( m_pAVFormatContext == NULL )
    return eAVInvalidSequence;

  // Check using of dyn buffer
  if ( m_uiAVFlags & AV_USE_DYN_BUF_IO )
  {
    if ( avio_open_dyn_buf( &m_pAVFormatContext->pb ) != 0 )
    {
      //@todo error tracing
    }
  }  
  
  /** 
   * Write the trailer, if any.  the trailer must be written
   * before you close the CodecContexts open when you wrote the
   * header; otherwise write_trailer may try to use memory that
   * was freed on av_codec_close()
   */  
  iResult = av_write_trailer( m_pAVFormatContext );
  //@todo
  
  // Check using of dyn buffer
  if ( m_uiAVFlags & AV_USE_DYN_BUF_IO )
  {
    uint8_t *pTmpBuf  = NULL;
    int      iBufSize = 0;
    
    iBufSize = avio_close_dyn_buf( m_pAVFormatContext->pb, &pTmpBuf );
    if ( iBufSize > 0 )
    {
      if ( ofs_write( pTmpBuf, iBufSize ) == false )
      {
	//@todo error tracing
      }
      
      if ( ofs_close() == false )
      {
	//@todo error tracing
      }
    }
    
    av_free( pTmpBuf );
  }  
  
  // Release audio codec
  if ( m_iBAS >= 0 )
  {
    iResult = avcodec_close( m_pAVFormatContext->streams[m_iBAS]->codec );
    CHECK_AVRESULT( iResult )  
  }
  
  // Set to initial value.
  m_pAudioCodec = NULL;
  
  if ( m_iBVS >= 0 )
  {
    iResult = avcodec_close( m_pAVFormatContext->streams[m_iBVS]->codec );
    CHECK_AVRESULT( iResult )   
  }
    
  // Set to initial value.
  m_pVideoCodec = NULL;

  if ( !(m_pAVFormatContext->oformat->flags & AVFMT_NOFILE) ) 
  {
    // Check using of dyn buffer
    if ( m_uiAVFlags & AV_USE_DYN_BUF_IO )
    {
      //@todo
    }
    else
    {
      avio_flush( m_pAVFormatContext->pb );
      
      iResult = avio_close( m_pAVFormatContext->pb );
    }
    //@todo
  }
  
  avformat_free_context(m_pAVFormatContext);
  // Set to initial value.
  m_pAVFormatContext = NULL;
  
  m_iBAS = -1;
  m_iBVS = -1;
  
  return eAVSucceded;
}
  
bool     CAVOutputFile::isOpened() const
{
  return ( m_pAVFormatContext != NULL );
}

AVFormatContext* CAVOutputFile::getAVFormatContext() const
{
  return m_pAVFormatContext;
}

AVCodecContext*  CAVOutputFile::getAVCodecContext( AVMediaType avMediaType ) const
{
  switch ( avMediaType )
  {
    case AVMEDIA_TYPE_VIDEO:
    {
      if ( m_iBVS == -1 )
	return NULL;
      
      return m_pAVFormatContext->streams[m_iBVS]->codec;
    }; break;
    
    case AVMEDIA_TYPE_AUDIO:
    { 
      if ( m_iBAS == -1 )
	return NULL;
      
      return m_pAVFormatContext->streams[m_iBAS]->codec;
    }; break;
  }
  
  return NULL;
}

AVCodec*         CAVOutputFile::getAudioCodec() const
{
  return m_pAudioCodec;
}

AVCodec*         CAVOutputFile::getVideoCodec() const
{
  return m_pVideoCodec;
}

/* add a video output stream */
AVStream*        CAVOutputFile::add_video_stream( int iStreamIndex, enum CodecID codec_id, int profile )
{
  AVCodecContext * c  = NULL;
  AVStream *       st = NULL;

  // find the video encoder 
  m_pVideoCodec = avcodec_find_encoder(codec_id);
  if ( m_pVideoCodec == NULL )
  {
    fprintf(stderr, "Could not find video codec\n");
    return NULL;
  }
  
  st = avformat_new_stream( m_pAVFormatContext, m_pVideoCodec );
  if (!st) 
  {
    fprintf(stderr, "Could not alloc video stream\n");
    return NULL;
  }
  
  // Used by get default context in order to load default parameters
  // for the specified codec.
  st->codec->codec_id = codec_id;

  avcodec_get_context_defaults3( st->codec, getVideoCodec() );
  
  // We need to update this value again for the reason that 
  // avcodec_get_context_defaults3 set this value to CODEC_ID_NONE
  st->codec->codec_id = codec_id;
  

  c                = st->codec;
  c->codec_id      = codec_id;
  c->codec_type    = AVMEDIA_TYPE_VIDEO;

  /* put sample parameters */
  c->bit_rate      = m_dstBitRate;
  /* resolution must be a multiple of two */
  c->width         = m_dstWidth;
  c->height        = m_dstHeight;
  /* time base: this is the fundamental unit of time (in seconds) in terms
    of which frame timestamps are represented. for fixed-fps content,
    timebase should be 1/framerate and timestamp increments should be
    identically 1. */
  c->time_base.den = m_dstFPS;
  c->time_base.num = 1;
  c->gop_size      = m_dstGOP; /* emit one intra frame every twelve frames at most */
  c->pix_fmt       = m_dstFormat;

  if (c->codec_id == CODEC_ID_MPEG2VIDEO)
  {
      /* just for testing, we also add B frames */
      c->max_b_frames = 2;
  }
  if (c->codec_id == CODEC_ID_MPEG1VIDEO)
  {
      /* Needed to avoid using macroblocks in which some coeffs overflow.
	This does not happen with normal video, it just happens here as
	the motion of the chroma plane does not match the luma plane. */
      c->mb_decision=2;
  }

  c->profile = profile;

  // some formats want stream headers to be separate
  if(m_pAVFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
      c->flags |= CODEC_FLAG_GLOBAL_HEADER;

  return st;
}

/*
* add an audio output stream
*/
AVStream* CAVOutputFile::add_audio_stream( int iStreamIndex, enum CodecID codec_id, int profile )
{
  AVCodecContext * c  = NULL;
  AVStream *       st = NULL;

  // find the audio encoder 
  m_pAudioCodec = avcodec_find_encoder(codec_id);
  if ( m_pAudioCodec == NULL )
  {
    fprintf(stderr, "Could not find audio codec\n");
    return NULL;
  }
  
  st = avformat_new_stream( m_pAVFormatContext, m_pAudioCodec );
  if (!st) 
  {
    fprintf(stderr, "Could not alloc audio stream\n");
    return NULL;
  }

  // Used by get default context in order to load default parameters
  // for the specified codec.
  st->codec->codec_id = codec_id;

  avcodec_get_context_defaults3( st->codec, getAudioCodec() );
  
  // We need to update this value again for the reason that 
  // avcodec_get_context_defaults3 set this value to CODEC_ID_NONE
  st->codec->codec_id = codec_id;
  
  c                 = st->codec;
  c->codec_id       = codec_id;
  c->codec_type     = AVMEDIA_TYPE_AUDIO;
  c->codec_tag      = av_codec_get_tag(m_pAVFormatContext->oformat->codec_tag, c->codec_id ); 

  // put sample parameters 
  c->sample_fmt     = AV_SAMPLE_FMT_S16;
  c->bit_rate       = 64000;
  c->sample_rate    = 16000;
  c->time_base      = (AVRational){1, c->sample_rate};
  c->channels       = 1;
  c->profile        = profile;
  c->block_align    = 0;
  
  // some formats want stream headers to be separate
  if(m_pAVFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
    c->flags |= CODEC_FLAG_GLOBAL_HEADER;

  return st;
}

bool	CAVOutputFile::ofs_open( const char* pFilename )
{
  bool _bRetVal = true;
  
  FFile* _pFile = new FFile( NULL, pFilename );
      
  FTRY
  {
    _pFile->Open( FWRITE_OPEN, 0, FCREATE_ALWAYS, 0 );
  }
  FCATCH( FFileSystemException, ex )
  {
    //@todo TRACE_EXCEPTION_CATCH( ex, ofs_open() );

    _bRetVal = false;
  }
  
  if ( m_lstOutputFiles == NULL )
    m_lstOutputFiles = new FTList< FFile* >();
  
  m_lstOutputFiles->PushTail( _pFile );
  
  return _bRetVal;
}

bool	CAVOutputFile::ofs_write( uint8_t *pBuffer, int iBufSize )
{
  bool _bRetVal = true;
  
  FTList<FFile* >::Iterator  _iter = m_lstOutputFiles->Begin();
  while ( _iter == TRUE )
  {
    FFile* _of = *_iter;
    
    FTRY
    {
      _of->Write( pBuffer, iBufSize );
    }
    FCATCH( FFileSystemException, ex )
    {
      //@todo TRACE_EXCEPTION_CATCH( ex, ofs_write() );
      
      _bRetVal = false;
    }
    
    _iter++;
  }

  return _bRetVal;
}

bool	CAVOutputFile::ofs_close()
{
  bool _bRetVal = true;

  while ( m_lstOutputFiles->IsEmpty() == false )
  {
    FFile* _of = m_lstOutputFiles->PopHead();
  
    FTRY
    {
      _of->Flush();
      _of->Close();
    }
    FCATCH( FFileSystemException, ex )
    {
      //@todo TRACE_EXCEPTION_CATCH( ex, ofs_close() );
      
      _bRetVal = false;
    }
    
    delete _of;
  }

  delete m_lstOutputFiles;
  m_lstOutputFiles = NULL;
  
  return _bRetVal;
}

}//namespace libavcpp

