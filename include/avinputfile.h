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


#ifndef AVINPUTFILE_H
#define AVINPUTFILE_H

#include "avfile.h"
#include <queue>

extern "C"
{
#include <libavutil/pixfmt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


using namespace std;

namespace libavcpp
{

/**
 * Forward declaration for AVRawFrame
 */
struct AVRawFrame;
  
  
class CAVInputFile : public CAVFile
{
public:
  /**
   */
  CAVInputFile();  
  /**
   */
  virtual ~CAVInputFile();

  /**
   *  Open specified file and allocate all resources needed for reading it.
   *
   *  @param pFilename   input file. Method will open the streaming file and setup
   *                     the best available codec for reading it.
   */
  virtual AVResult open( const char* pFilename, unsigned int uiAVFlags = AV_SET_BEST_VIDEO_CODEC );
  /**
   */
  virtual AVResult read( AVStream*& pAVStream, AVPacket*& pAVPacket, bool bBuffering );
  
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
   * 
   */
  AVCodec*	   getAudioCodec() const;
  /**
   * 
   */
  AVCodec*	   getVideoCodec() const;
  
protected:
  /***/
  virtual AVResult write( AVMediaType avMediaType, AVPacket& rAVPacket, unsigned int uiAVFlags )
  { return eAVSucceded; }
  /***/
  virtual AVResult flush( unsigned int uiAVFlags )
  { return eAVSucceded; }

private:
  unsigned int 	        m_uiAVFlags;
  AVFormatContext*	m_pAVFormatContext;
  int                   m_iBAS;               // Best Audio Stream
  int                   m_iBVS;               // Best Video Stream
  AVDictionary* 	m_pOptBAS; 
  AVDictionary* 	m_pOptBVS; 
  AVCodec*              m_pAudioCodec; 
  AVCodec*              m_pVideoCodec;
  bool                  m_bReachedEOF;
  AVRawFrame* 		m_pTopRawFrame;	     // Top frame used to avoid memory free and allocation.
  queue< AVRawFrame* >  m_queueInputFrames;
};

}//namespace libavcpp

#endif // AVINPUTFILE_H
