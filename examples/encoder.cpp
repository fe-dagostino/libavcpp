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


#include <iostream>

#include "./include/avapplication.h"
#include "./include/avdecoder.h"
#include "./include/avencoder.h"
#include "./include/avimage.h"

using namespace libavcpp;


class AVDecoderEventsImp : public IAVDecoderEvents
{
public:
  AVDecoderEventsImp( const char* pFilename, const char* pBackground )
    : m_iVideoFrame(0), m_bStartRec( false )
  {
    std::cout << "Open Encoder" << std::endl;
    if ( m_avEncoder.open( pFilename, 0, 1366, 768, PIX_FMT_YUV420P, 30/*FPS*/, 10/*GOP*/, 4000000/*bit rate*/, CODEC_ID_MPEG4, FF_PROFILE_MPEG4_ADVANCED_REAL_TIME ) != eAVSucceded )
    {
      std::cout << "Failed to open Encoder" << std::endl;
      exit(-1);
    }
   
    if ( m_avBackground.load( pBackground, -1, -1, PIX_FMT_RGBA ) != eAVSucceded )
    {
      std::cout << "Failed to load background image" << std::endl;
      exit(-1);
    }
  }
  
  virtual ~AVDecoderEventsImp()
  {
    std::cout << "Close Encoder" << std::endl;
    m_avEncoder.close();
  }
  
  const CAVImage*      getImage() const
  { 
    return &m_avImage;
  }
  
  int  getFrameCount() const
  {
    return m_iVideoFrame;
  }

  virtual void   OnVideoKeyFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecCtx, double time )
  {
    m_bStartRec = true;
    m_dStart    = time;
  }
  
  virtual bool   OnVideoFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecCtx, double time )
  {
    if ( m_bStartRec = true )
    {
      // Initialize background
      m_avBkgDB.init( m_avBackground, -1, -1, m_avBackground.getFormat() );

      int frameW = 576;
      int frameH = 320;
      int dstPosX = 700;
      int dstPosY = 406;
      
      printf( "Input Video Frame [%d] %dx%d\n", m_iVideoFrame, pAVFrame->owner->width, pAVFrame->owner->height );
      if ( m_avInputImage.init( pAVFrame, pAVCodecCtx, frameW, frameH, PIX_FMT_RGB24 ) != eAVSucceded )
      {
	  std::cout << "Error on m_avImage.init()" << std::endl;
      }
      
      m_avBkgDB.blend( CAVPoint( dstPosX, dstPosY ), m_avInputImage );

      // Preparing output frame in YUV
      m_avImage.init( m_avBkgDB, -1, -1, PIX_FMT_YUV420P ); 
      
      CAVFrame  _avFrame( &m_avImage );
      if ( m_avEncoder.write( &_avFrame, 0 ) != eAVSucceded )
      {
	std::cout << "Failed to write frame=" << std::endl;
      }
      _avFrame.detach();
      
      
      if ( m_iVideoFrame++ > 5000 )
      {
	return false;
      }
    }
    return true;
  }
  
  virtual bool   OnFilteredVideoFrame( const AVFilterBufferRef* pAVFilterBufferRef, const AVStream* pAVStream, const AVCodecContext* pAVCodecCtx, double pst )
  {
    /* Nothing to do */  
  }
  
  virtual bool   OnAudioFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecCtx, double time )
  {
    return true;
  }
  
private:
  bool             m_bStartRec;
  double           m_dStart;
  int              m_iVideoFrame;
  CAVImage         m_avBackground;
  CAVImage         m_avBkgDB;
  CAVImage         m_avInputImage;
  CAVImage         m_avImage;
  CAVEncoder       m_avEncoder;
};


int main(int argc, char **argv)
{
    if ( argc < 4 )
    {
      printf( "Eecoder\n" );
      printf( "Example application demostrating how to read/write stream.\n" );
      printf( "\nUsage:\n" );
      printf( "\t./encoder <source> <destination> <background>\n" );
      printf( "\n <source>       source video should be file in order to terminate.\n" );
      printf( "\n <destination>  destination file name.\n" );
      printf( "\n <background>   png with trasparent area for alpha-blending.\n" );
      exit(0);      
    }
    
    CAVApplication::initLibAVCPP();

    CAVDecoder         _avDecoder;
    AVDecoderEventsImp _avDecoderEvents( argv[2], argv[3] );
    _avDecoder.setDecoderEvents( &_avDecoderEvents, false );

    // Track start time
    time_t sTime = time( NULL );
    
    if ( _avDecoder.open( argv[1], 0.0 ) == eAVSucceded )
      std::cout << "Open=[OK]" << std::endl;
    else
    {
      std::cout << "Open=[KO]" << std::endl;
      return -1;
    }
    
    std::cout << "Start Decoding" << std::endl;
    _avDecoder.read();
    std::cout << "End   Decoding" << std::endl;
    
    std::cout << "Close Decoder" << std::endl;
    if ( _avDecoder.close() != eAVSucceded )
    {
      std::cout << "Decoder Close ERROR" << std::endl;
    }
    std::cout << "Decoder Closed" << std::endl;
    
    // Track end time
    time_t eTime = time( NULL );
    
    time_t dTime = eTime - sTime;
    
    float  fps = _avDecoderEvents.getFrameCount()  / ((float)dTime);
    std::cout << "FPS=" << fps << std::endl;
 
    CAVApplication::deinitLibAVCPP();
    
    return 0;
}
  