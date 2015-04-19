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
  
  virtual void   OnVideoKeyFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecCtx, double time )
  {
  }
  
  virtual bool   OnVideoFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecCtx, double time )
  {
     std::cout << "Got Frame" << std::endl;
     
     m_avImage.init( pAVFrame, pAVCodecCtx, -1, -1, PIX_FMT_RGB24 );
     std::cout << "Video Frame=" << m_iVideoFrame++ << " W=" << m_avImage.getWidth() << " H=" << m_avImage.getHeight() << " LS=" << m_avImage.getLineSize( 0 ) << std::endl;
     m_avImage.saveToPPM( "image.ppm" );
     
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
  int              m_iVideoFrame;
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
      printf( "Image To PPM (Portable Pixel Image File)\n" );
      printf( "Utility to transform a generic input image file png,jpeg .. to PPM \n" );
      printf( "\nUsage:\n" );
      printf( "\t./img2ppm <imagefile>\n" );
      printf( "In case of success and output file image.ppm will be created.\n" );
      exit(0);
    }
    else
      pFileName = argv[1];      
    
    std::cout << "Filename=" << pFileName << std::endl;
    if ( _avDecoder.open( pFileName , 0.0 ) == eAVSucceded )
      std::cout << "Open=[OK]" << std::endl;
    else
      std::cout << "Open=[KO]" << std::endl;
    
    std::cout << "Start Decoding" << std::endl;
    _avDecoder.read( AVD_EXIT_ON_VIDEO_KEY_FRAME );
    
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
  
