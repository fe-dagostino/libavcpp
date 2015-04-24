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


#ifndef AVOUTPUTFILE_H
#define AVOUTPUTFILE_H

#include "FTList.tlh"
#include "FFile.h"

#include "avfile.h"
extern "C"
{
#include <libavutil/pixfmt.h>
#include <libavcodec/avcodec.h>
}

struct AVStream;

namespace libavcpp
{

class CAVImage;


class CAVOutputFile : public CAVFile
{
public:
  class CAVStream
  {
    public:
      CAVStream()
        : m_mediaType( AVMEDIA_TYPE_UNKNOWN ), m_codecID( AV_CODEC_ID_NONE ), m_iProfile( 0 )
      {}
      
      CAVStream( AVMediaType mediaType, AVCodecID codecID, int iProfile )
        : m_mediaType( mediaType ), m_codecID( codecID ), m_iProfile( iProfile )
      {}

      inline AVMediaType  getMediaType() const
      { return m_mediaType; }
      
      inline AVCodecID    getCodecID() const
      { return m_codecID;   }
      
      inline int          getProfile() const
      { return m_iProfile;   }
  
    private:
      AVMediaType  m_mediaType;
      AVCodecID    m_codecID;
      int          m_iProfile;
  };
  
public:
  /**
   * @param pDstStreams must be allocated with 'new' and terminated with CAVStream() defalut constructor. 
   */
  CAVOutputFile( 
		  int         dstWidth, 
		  int         dstHeight,
		  PixelFormat dstFormat,
		  int         dstFPS,
		  int         dstGOP,
		  int         dstBitRate,
		  CAVStream*  pDstStreams 
	       );  
  /**
   */
  virtual ~CAVOutputFile();

  /**
   *  Open specified file and allocate all resources needed for writing it.
   * 
   *  @param pFilename destination output file. File extension will be used to autodetect 
   *                  output file format. Default output format is "mpeg".
   *  @param uiAVFlags set some option for the output file. valid flags are the following
   *                   #define AV_INTERLEAVED_AUDIO_WR 0x00000004
   *                   #define AV_INTERLEAVED_VIDEO_WR 0x00000008
   *                   #define AV_ENCODE_AUDIO_STREAM  0x00000010
   *                   #define AV_ENCODE_VIDEO_STREAM  0x00000020
   */
  virtual AVResult open( const char* pFilename, unsigned int uiAVFlags );
  /**
   */
  virtual AVResult write( AVMediaType avMediaType, AVPacket& rAVPacket, unsigned int uiAVFlags );
  /**
   */
  virtual AVResult flush( unsigned int uiAVFlags );

  /**
   */
  virtual AVResult close();
  /**
   *  Check if output file is currently opened or not.
   *  @return true if it is opened, false otherwise.
   */
  virtual bool     isOpened() const;
  
  /**
   *  Return uiFlags value used during open operation.
   */
  virtual unsigned int   getFlags() const
  { return m_uiAVFlags; }
 
  /**
   */
  AVFormatContext* getAVFormatContext() const;
  
  /**
   */
  AVCodecContext*  getAVCodecContext( AVMediaType avMediaType ) const;

  /**
   * 
   */
  AVCodec*	   getAudioCodec() const;
  /**
   * 
   */
  AVCodec*	   getVideoCodec() const;
  
private:

private:
  AVStream*        add_video_stream( int iStreamIndex, enum AVCodecID codec_id, int profile );

  AVStream*        add_audio_stream( int iStreamIndex, enum AVCodecID codec_id, int profile );

  bool             ofs_open( const char* pFilename );
  bool             ofs_write( uint8_t *pBuffer, int iBufSize );
  bool             ofs_close();
  
private:
  unsigned int 	        m_uiAVFlags;
  CAVStream*            m_pDstStreams;
  AVFormatContext*	m_pAVFormatContext;
  int                   m_dstWidth;
  int                   m_dstHeight;
  PixelFormat 		m_dstFormat;
  int                   m_dstFPS;
  int                   m_dstGOP;
  int                   m_dstBitRate;
  int                   m_iBAS;               // Best Audio Stream
  int                   m_iBVS;               // Best Video Stream
  AVDictionary* 	m_optBAS; 
  AVDictionary* 	m_optBVS;
  AVCodec*              m_pAudioCodec; 
  AVCodec*              m_pVideoCodec;
  FTList<FFile* >*      m_lstOutputFiles;
};

}//namespace libavcpp

#endif // AVOUTPUTFILE_H
