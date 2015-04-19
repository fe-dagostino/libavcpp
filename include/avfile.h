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


#ifndef AVFILE_H
#define AVFILE_H

#include "avcommon.h"
#include "FMutex.h"

extern "C"
{
#include <libavutil/avutil.h>
}

struct AVFormatContext;
struct AVCodecContext;
struct AVCodec;
struct AVPacket;
struct AVFrame;

USING_NAMESPACE_FED

namespace libavcpp
{

/** Open Flags
 */
#define AV_SET_BEST_AUDIO_CODEC 0x00000001
#define AV_SET_BEST_VIDEO_CODEC 0x00000002
#define AV_ENCODE_AUDIO_STREAM  0x00000004
#define AV_ENCODE_VIDEO_STREAM  0x00000008

/** Write Flags
 */
#define AV_INTERLEAVED_AUDIO_WR 0x00000010
#define AV_INTERLEAVED_VIDEO_WR 0x00000020

/** Flush Flags
 */

/** IO Flags
 */
#define AV_USE_DYN_BUF_IO       0x00000080

/**
 * Interface class for Audio Video Files.
 */
class CAVFile
{
public:
  /**
   * Empty constructor.
   */
  CAVFile();  
  /**
   * Destructor.
   */
  virtual ~CAVFile();

  /**
   * Open specified file and allocate all resources needed for reading and/or writing.
   */
  virtual AVResult open( const char* pFilename, unsigned int uiAVFlags ) = 0;
  /**
   * Read next audio video packet.
   */
  virtual AVResult read( AVCodecContext*& pAVCodecContext, AVPacket*& pAVPacket, bool bBuffering ) {};
  /**
   * Write an audio video packet.
   */
  virtual AVResult write( AVMediaType avMediaType, AVPacket& rAVPacket, unsigned int uiAVFlags ) {};
  /**
   */
  virtual AVResult flush( unsigned int uiAVFlags ) {};
  /**
   * Close file and release all alloated resources..
   */
  virtual AVResult close() = 0;
  /**
   *  Check if output file is currently opened or not.
   *  @return true if it is opened, false otherwise.
   */
  virtual bool           isOpened() const = 0;
  
  /**
   *  Return uiFlags value used during open operation.
   */
  virtual unsigned int   getFlags() const = 0;
  
protected:
  /***/
  void     addInstance( int iCounter );
  
  /***/
  void     delInstance( int iCounter );
  
protected:
  
  FMutex&  getMutex();
  
private:
};

}//namespace libavcpp

#endif // AVFILE_H
