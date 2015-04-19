#include <iostream>

#include "./include/avapplication.h"
#include "./include/avdecoder.h"
#include "./include/avimage.h"

using namespace libavcpp;


class AVDecoderEventsImp : public IAVDecoderEvents
{
public:
  AVDecoderEventsImp()
    : m_iVideoFrame(0)
  {
    
  }
  
  const CAVImage*      getImage() const
  { 
    return &m_avImage;
  }
  
  int  getFrameCount() const
  {
    return m_iVideoFrame;
  }
  
  virtual void   OnVideoKeyFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecContext, double time )
  {
    
  }
  
  virtual bool   OnVideoFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecContext, double time )
  {
     m_avImage.init( pAVFrame, pAVCodecContext, -1, -1 );
     printf( "Video Frame=%d W=%d H=%d LS=%d time=%f\n", m_iVideoFrame++, m_avImage.getWidth(), m_avImage.getHeight(), m_avImage.getLineSize( 0 ), time );
     return true;
  }
  
  virtual bool   OnFilteredVideoFrame( const AVFilterBufferRef* pAVFilterBufferRef, const AVStream* pAVStream, const AVCodecContext* pAVCodecContext, double pst )
  {
    /* Nothing to do */  
  }
  
  virtual bool   OnAudioFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecContext, double time )
  {
     printf( "Audio Frame=%d time=%f\n", m_iAudioFrame++, time );
     return true;
  }

private:
  int              m_iVideoFrame;
  int              m_iAudioFrame;
  CAVImage         m_avImage;
};

int main(int argc, char **argv)
{
    CAVApplication::initLibAVCPP();
    
    CAVDecoder         _avDecoder;
    AVDecoderEventsImp _avDecoderEvents;
    _avDecoder.setDecoderEvents( &_avDecoderEvents, false );

    time_t sTime = time( NULL );
    const char* pFileName = NULL;
    
    if ( argc < 2 )
    {
      printf( "Decoder\n" );
      printf( "Example application demostrating how to read from a/v stream.\n" );
      printf( "\nUsage:\n" );
      printf( "\t./decoder <file video or device>\n" );
      exit(0);      
    }
    else
      pFileName = argv[1];    
    
    std::cout << "Filename=" << pFileName << std::endl;
    if ( _avDecoder.open( pFileName, 60.0, AV_SET_BEST_AUDIO_CODEC|AV_SET_BEST_VIDEO_CODEC ) == eAVSucceded )
      std::cout << "Open=[OK]" << std::endl;
    else
      std::cout << "Open=[KO]" << std::endl;
    
    std::cout << "Start Decoding" << std::endl;
    _avDecoder.read();
    std::cout << "End   Decoding" << std::endl;
    
    std::cout << "Close Decoder" << std::endl;
    if ( _avDecoder.close() != eAVSucceded )
    {
      std::cout << "Decoder Close ERROR" << std::endl;
    }
    std::cout << "Decoder Closed" << std::endl;
    
    time_t eTime = time( NULL );
    
    time_t dTime = eTime - sTime;
    
    float  fps = _avDecoderEvents.getFrameCount()  / ((float)dTime);
    std::cout << "FPS=" << fps << std::endl;

    CAVApplication::deinitLibAVCPP();
    
    return 0;
}
  