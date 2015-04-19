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


#ifndef AVDECODER_H
#define AVDECODER_H

#include "avcommon.h"
#include "avinputfile.h"
#include "avfiltergraph.h"

namespace libavcpp
{

class IAVDecoderEvents
{
public:
    /**
     */
    virtual void   OnVideoPacket( const AVPacket* pAVPacket, const AVStream* pAVStream, const AVCodecContext* pAVCodecContext ) {};
    /**
     */
    virtual void   OnAudioPacket( const AVPacket* pAVPacket, const AVStream* pAVStream, const AVCodecContext* pAVCodecContext ) {};
    
    /**
     *  Event will be raised just in presence of one key frame.
     *  In presence of key frame events will be raised in the following sequence
     *  OnVideoKeyFrame() and after that OnVideoFrame(). 
     */
    virtual void   OnVideoKeyFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecContext, double pts ) = 0;
  
    /**
     *  Event will be raised for each frame coming from the stream.
     *  Return value true in order to continue decoding, false to interrupt.
     *  Note: this event will be raised also for each key frame.
     */
    virtual bool   OnVideoFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecContext, double pts ) = 0;
    
    /**
     *  Event will be raised for each frame coming from the stream.
     *  Return value true in order to continue decoding, false to interrupt.
     *  Note: this event will be raised for each frame.
     */
    virtual bool   OnFilteredVideoFrame( const AVFilterBufferRef* pAVFilterBufferRef, const AVStream* pAVStream, const AVCodecContext* pAVCodecContext, double pts ) = 0;
    
    /**
     *  Event will be raised for each frame coming from the stream.
     *  Return value true in order to continue decoding, false to interrupt.
     */
    virtual bool   OnAudioFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecContext, double pts ) = 0;
    
};

/**
 * Flags to be used with CAVDecoder.Read
 */
#define AVD_EXIT_ON_NEXT_AUDIO_FRAME  0x00000001  // Read streaming until next audio frame will be collected and then exit.
#define AVD_EXIT_ON_NEXT_VIDEO_FRAME  0x00000002  // Read streaming until next video frame will be collected and then exit.
#define AVD_EXIT_ON_VIDEO_KEY_FRAME   0x00000004  // Read streaming until next video KEY frame will be collected and then exit.
                                                 // This flas is incompatible with AV_EXIT_ON_NEXT_VIDEO_FRAME, so in case both
                                                 // the flags will be set to ON just the AV_EXIT_ON_VIDEO_KEY_FRAME will be 
                                                 // evaluated and AV_EXIT_ON_NEXT_VIDEO_FRAME will be ignored.
                                                 // Note: that OnVideoFrame() will be invoked for each Video Frame even it is
                                                 // not a key frame.
#define AVD_EXIT_ON_BUFFERING         0x00000008 // If specified read will exit on each new packet added into buffer and return
                                                 // value will be eAVBuffering. If not specified funcitn will exit when buffering
						 // will be completed and return value will be eAVSucceded.

class CAVDecoder
{

public:
  /**
   * Empty constructor.
   */
  CAVDecoder( );
  /**
   * Destructor method. 
   * Automatically release all resources, so if decoder status
   * is still open it will be closed during destroy.
   */
  virtual ~CAVDecoder();

  /**
   *  Set event handler for decoder.
   *  @param pEvents      instance to @IAVDecoderEvents object.
   *                      NULL value will be also accepted.
   *  @param bAutoRelease if true pEvents object will be automatically released 
   *                      when decoder will be destroyed.
   */
  virtual AVResult setDecoderEvents( IAVDecoderEvents* pEvents, bool bAutoRelease = true );
  /** 
   *  Retrieve current reference to @IAVDecoderEvents object.
   *  @param pEvents populated with object address. 
   */
  virtual AVResult getDecoderEvents( IAVDecoderEvents*& pEvents );
  /**
   *  Open decoder on specified file.
   *  @param pFilename filename
   *  @param dBufferTime time in seconds to be buffered before raising events.
   */
  virtual AVResult open( const char* pFilename, double dBufferTime, unsigned int uiAVFlags = AV_SET_BEST_VIDEO_CODEC );
  
  /**
   *  Read frames.
   *  By default this function will block executing thread until end of file will be reached,
   *  however it is possible to control execution using some @wFlags in order to go forward to
   *  next frame or to next key frame.
   */
  virtual AVResult read( unsigned int wFlags = 0 );
  /**
   *  Close opened file and release all resources.
   */
  virtual AVResult close();
  
  /**
   *  Check if decoder is currently open or not.
   *  @return true if decoder is open, false otherwise
   */
  virtual bool     isOpened() const;
  
  /**
   */
  virtual CAVFilterGraph* setFilterGraph( CAVFilterGraph* pAVFilterGraph );
  
  /**
   */
  virtual CAVFilterGraph* getFilterGraph() const;
  

  /**
   */
  static double    getTime();
  
private:
  IAVDecoderEvents*  m_pEvents;
  bool 		     m_bAutoRelease;
  CAVInputFile       m_avInputFile;
  double             m_dStartTime;
  double             m_dBufferTime;
  mutable FMutex     m_mtxFilterGraph;
  CAVFilterGraph*    m_pAVFilterGraph;
};

}//namespace libavcpp

#endif // AVDECODER_H
