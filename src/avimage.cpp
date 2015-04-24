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


#include "../include/avimage.h"
#include "../include/avdecoder.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
}

namespace libavcpp
{

/** 
 * Got from avplay in livav library.
 */
#define ALPHA_BLEND(a, oldp, newp, s)\
((((oldp << s) * (255 - (a))) + (newp * (a))) / (255 << s))  
  

class AVDecoderEventsImp : public IAVDecoderEvents
{
public:
  AVDecoderEventsImp( 
                      CAVImage&        rAVImage, 
		      int 	       iWidth,
		      int 	       iHeight,
		      enum PixelFormat imgFormat 
		    )
    : m_rAVImage( rAVImage ), m_iWidth( iWidth ), m_iHeight( iHeight ), m_pixFormat( imgFormat )
  {
    
  }
  
  virtual void   OnVideoKeyFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecContext, double time )
  {
    m_rAVImage.init( pAVFrame, pAVStream->codec, m_iWidth, m_iHeight, m_pixFormat );
  }
  
  virtual bool   OnVideoFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecContext, double time )
  {  
    /* Nothing to do */  
    return true;
  }
  
  virtual bool   OnFilteredVideoFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecContext, double pst )
  {
    /* Nothing to do */  
    return true;
  }
  
  virtual bool   OnAudioFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecContext, double time )
  {  
    /* Nothing to do */  
    return true;
  }
  
  virtual bool   OnFilteredAudioFrame( const AVFrame* pAVFrame, const AVStream* pAVStream, const AVCodecContext* pAVCodecContext, double pst )
  {
    /* Nothing to do */  
    return true;
  }
  
private:
    PixelFormat  m_pixFormat;
    int 	 m_iWidth;
    int 	 m_iHeight;
    CAVImage&    m_rAVImage;
};
  
  
  
  
CAVImage::CAVImage()
        : m_pSwsContext( NULL ),
        m_iSize( -1 ),
        m_pFrame( NULL ),
        m_iWidth( -1 ),
        m_iHeight( -1 ),
        m_eFormat( PIX_FMT_NONE )
{

}

CAVImage::CAVImage( const CAVImage& rImage )
        : m_pSwsContext( NULL ),
        m_iSize( -1 ),
        m_pFrame( NULL ),
        m_iWidth( -1 ),
        m_iHeight( -1 ),
        m_eFormat( PIX_FMT_NONE )
{
    init( rImage, -1, -1, rImage.getFormat() );
}


CAVImage::~CAVImage()
{
    free();
}

const unsigned char* CAVImage::getData( int iPlane, int* pSize ) const
{
    if ( pSize != NULL )
        *pSize = getLineSize(iPlane)*getHeight();

    if ( m_pFrame == NULL )
      return NULL;
      
    return m_pFrame->data[iPlane];
}

unsigned char*       CAVImage::getData( int iPlane, int* pSize )
{
    if ( pSize != NULL )
        *pSize = getLineSize(iPlane)*getHeight();

    if ( m_pFrame == NULL )
      return NULL;
      
    return m_pFrame->data[iPlane];
}

CAVRect              CAVImage::getRect() const
{
  return CAVRect( 0, 0, getWidth(), getHeight() );
}

int                  CAVImage::getWidth() const
{
    if ( m_pFrame == NULL )
      return -1;
    
    return m_iWidth;
}
  
int                  CAVImage::getHeight() const
{
    if ( m_pFrame == NULL )
      return -1;
  
    return m_iHeight;
}

enum PixelFormat     CAVImage::getFormat() const
{
  return m_eFormat;
}

AVResult    CAVImage::init( const AVFrame* pFrame, 
			    const AVCodecContext* pAVCodecCtx, 
			    int dstW, int dstH, 
			    PixelFormat dstFormat,
			    int flags
			   )
{
    // Check Codec Context to be valid.
    if ( pAVCodecCtx == NULL )
        return eAVInvalidParameters;
    
    // Check if input format is supported
    if ( sws_isSupportedInput( pAVCodecCtx->pix_fmt ) == 0 )
        return eAVUnsupportedInputFormat;

    // Check if output format is supported
    if ( sws_isSupportedOutput( dstFormat ) == 0 )
	return eAVUnsupportedOutputFormat;

    dstW = (dstW==-1)?pAVCodecCtx->width:dstW;
    dstH = (dstH==-1)?pAVCodecCtx->height:dstH;
    
    m_pSwsContext = sws_getCachedContext (
                        m_pSwsContext,
                        pAVCodecCtx->width,
                        pAVCodecCtx->height,
                        pAVCodecCtx->pix_fmt,
                        dstW,
                        dstH,
                        dstFormat,
                        flags,
                        NULL, NULL, NULL
                    );

    int iReqiredSize = avpicture_get_size( dstFormat, dstW, dstH );
    if ( iReqiredSize == -1 )
        return eAVTooLargeBufferRequired;

    // Check if the buffer need to be reallocated.
    if ( iReqiredSize > m_iSize )
    {
        // Release previous allocated buffer.
        if ( m_pFrame != NULL )
        {
            avpicture_free( (AVPicture*)m_pFrame );
            m_pFrame = NULL;
            m_iSize = -1;
        }

        // Allocate new buffer.
	m_pFrame = av_frame_alloc();
        if ( m_pFrame == NULL )
            return eAVTooLargeBufferRequired;

	// Initialize destination frame with buffer, format and size.
	if ( avpicture_alloc( (AVPicture*)m_pFrame, dstFormat, dstW, dstH ) < 0 )
	    return eAVTooLargeBufferRequired;
	
        // Set new buffer size.
        m_iSize = iReqiredSize;
    }

    int finalH = sws_scale( m_pSwsContext, 
	       pFrame->data, pFrame->linesize, 0, pAVCodecCtx->height,
	       m_pFrame->data, m_pFrame->linesize 
 	    );

    m_iWidth  = dstW;
    m_iHeight = dstH;
    m_eFormat = dstFormat;
    
    return eAVSucceded;
}

AVResult    CAVImage::init( const CAVImage& rImage, 
			    int dstW, int dstH, 
			    PixelFormat dstFormat,
			    int flags
			    )
{
    // Check if input format is supported
    if ( sws_isSupportedInput( rImage.getFormat() ) == 0 )
        return eAVUnsupportedInputFormat;

    // Check if output format is supported
    if ( sws_isSupportedOutput( dstFormat ) == 0 )
	return eAVUnsupportedOutputFormat;

    dstW = (dstW==-1)?rImage.getWidth():dstW;
    dstH = (dstH==-1)?rImage.getHeight():dstH;
    
    m_pSwsContext = sws_getCachedContext (
                        m_pSwsContext,
                        rImage.getWidth(),
                        rImage.getHeight(),
                        rImage.getFormat(),
                        dstW,
                        dstH,
                        dstFormat,
                        flags,
                        NULL, NULL, NULL
                    );

    int iReqiredSize = avpicture_get_size( dstFormat, dstW, dstH );
    if ( iReqiredSize == -1 )
        return eAVTooLargeBufferRequired;

    // Check if the buffer need to be reallocated.
    if ( iReqiredSize > m_iSize )
    {
        // Release previous allocated buffer.
        if ( m_pFrame != NULL )
        {
            av_free( m_pFrame );
            m_pFrame = NULL;
            m_iSize = -1;
        }

        // Allocate new buffer.
	m_pFrame = av_frame_alloc();
        if ( m_pFrame == NULL )
            return eAVTooLargeBufferRequired;

	// Initialize destination frame with buffer, format and size.
	if ( avpicture_alloc( (AVPicture*)m_pFrame, dstFormat, dstW, dstH ) < 0 )
	    return eAVTooLargeBufferRequired;
	
        // Set new buffer size.
        m_iSize = iReqiredSize;
    }

    int finalH = sws_scale( m_pSwsContext, 
	       rImage.m_pFrame->data, rImage.m_pFrame->linesize, 0, rImage.getHeight(),
	       m_pFrame->data, m_pFrame->linesize 
 	    );

    m_iWidth  = dstW;
    m_iHeight = dstH;
    m_eFormat = dstFormat;
    
    return eAVSucceded;
}

AVResult    CAVImage::init( 
			    const AVFilterBufferRef* pAVFilterBufferRef,
			    int srcW, int srcH
			  )
{
  return init( pAVFilterBufferRef, srcW, srcH, srcW, srcH, (PixelFormat)pAVFilterBufferRef->format ); 
}

AVResult    CAVImage::init( const AVFilterBufferRef* pAVFilterBufferRef,
			    int srcW, int srcH,
			    int dstW, int dstH, 
			    PixelFormat dstFormat,
			    int flags )
{
  if ( pAVFilterBufferRef == NULL )
    return eAVInvalidParameters;
    
  // Check if input format is supported
  if ( sws_isSupportedInput( (PixelFormat)pAVFilterBufferRef->format ) == 0 )
    return eAVUnsupportedInputFormat;

  // Check if output format is supported
  if ( sws_isSupportedOutput( dstFormat ) == 0 )
    return eAVUnsupportedOutputFormat;  
  
  dstW = (dstW==-1)?srcW:dstW;
  dstH = (dstH==-1)?srcH:dstH;
    
  m_pSwsContext = sws_getCachedContext (
		      m_pSwsContext,
		      srcW,
		      srcH,
		      (PixelFormat)pAVFilterBufferRef->format,
		      dstW,
		      dstH,
		      dstFormat,
		      flags,
		      NULL, NULL, NULL
		  );
    
    
  int iRequiredSize = avpicture_get_size( dstFormat, dstW, dstH );
  if ( iRequiredSize == -1 )
      return eAVTooLargeBufferRequired;

  // Check if the buffer need to be reallocated.
  if ( iRequiredSize > m_iSize )
  {
      // Release previous allocated buffer.
      if ( m_pFrame != NULL )
      {
	  av_free( m_pFrame );
	  m_pFrame = NULL;
	  m_iSize = -1;
      }

      // Allocate new buffer.
      m_pFrame = av_frame_alloc();
      if ( m_pFrame == NULL )
	  return eAVTooLargeBufferRequired;

      // Initialize destination frame with buffer, format and size.
      if ( avpicture_alloc( (AVPicture*)m_pFrame, dstFormat, dstW, dstH ) < 0 )
	  return eAVTooLargeBufferRequired;
      
      // Set new buffer size.
      m_iSize = iRequiredSize;
  }

  int finalH = sws_scale( m_pSwsContext, 
	      pAVFilterBufferRef->data, pAVFilterBufferRef->linesize, 0, srcH,
	      m_pFrame->data, m_pFrame->linesize 
	  );
    
  m_iWidth  = dstW;
  m_iHeight = dstH;
  m_eFormat = dstFormat;
  
  return eAVSucceded;  
}

void      CAVImage::free()
{
    if ( m_pSwsContext != NULL )
    {
        sws_freeContext( m_pSwsContext );
        m_pSwsContext = NULL;
    }
    
    // Release previous allocated buffer.
    if ( m_pFrame != NULL )
    {
	avpicture_free( (AVPicture*)m_pFrame );
	av_free( m_pFrame );
	
	m_pFrame  = NULL;
	m_iSize   = -1;
        m_iWidth  = -1;
        m_iHeight = -1;
	m_eFormat = PIX_FMT_NONE;
    }  
}

AVResult    CAVImage::saveToPPM( const char* pFilename )
{
    FILE           *pFile  = NULL;

    if ( m_pFrame == NULL )
      return eAVInvalidParameters;
    
    if ( m_eFormat != PIX_FMT_RGB24 )
      return eAVUnsupportedInputFormat;
    
    if ( pFilename == NULL )
      return eAVInvalidParameters;
    
    // Open file handler
    pFile=fopen(pFilename, "wb");
    if( pFile == NULL)
      return eAVIOError;

    // Write header with Width and Height and format.
    fprintf(pFile, "P6\n%d %d\n255\n", getWidth(), getHeight() );

    // Write pixel data
    for(register int h=0; h< getHeight(); h++)
    {
      fwrite( m_pFrame->data[0]+h*getLineSize(0), 1, getLineSize(0), pFile);
    }
    
    // Close file handler
    fclose(pFile);
    
    return eAVSucceded; 
}  

AVResult	CAVImage::load( 
                                const char* 	 pFilename, 
				int 	  	 iWidth,
				int 	  	 iHeight,			        
				enum PixelFormat eFormat 
			      )
{
    AVResult    _retVal;
    AVDecoderEventsImp  _events( *this, iWidth, iHeight, eFormat );
    CAVDecoder  	_decoder;

    // Set events handler
     _retVal = _decoder.setDecoderEvents( &_events, false );
    if ( _retVal != eAVSucceded )
      return _retVal;
    
    _retVal = _decoder.open( pFilename, 0.0, AV_SET_BEST_VIDEO_CODEC );
    if ( _retVal != eAVSucceded )
      return _retVal;

    _retVal = _decoder.read( AVD_EXIT_ON_VIDEO_KEY_FRAME );
    if ( _retVal != eAVSucceded )
      return _retVal;
    
    _retVal = _decoder.close();
    
    return _retVal; 
}

AVResult	CAVImage::blend( const CAVPoint& rPos, const CAVImage& rImage )
{
  if ( m_eFormat != PIX_FMT_RGBA )
    return eAVUnsupportedOperation;
  
  if (
       ( rImage.getFormat() != PIX_FMT_RGB24 ) &&
       ( rImage.getFormat() != PIX_FMT_RGBA  )
     )
    return eAVUnsupportedInputFormat;

 
  // @todo just process ovelapping rectangle.
  // rImage rect must overlap dest image
  
  int         iDstBuffer = 0;
  u_int8_t*   pDstBuffer = (u_int8_t*)getData( 0, &iDstBuffer );

  int         iSrcBuffer = 0;
  u_int8_t*   pSrcBuffer = (u_int8_t*)rImage.getData( 0, &iSrcBuffer );
  
  CAVRect  _dstRect = this->getRect();  // Calling methods on CAVRect is more efficient than calling 
                                        // methods on CAVImages.
  CAVRect  _srcRect = rImage.getRect(); // Source Rect in relarive coordinate system.
					
  int      _dstNdx; // will be used to keep destPos as array index.
  int      _srcNdx;
  
  register int      _dstOffsetX = rPos.getX() << 2;            //Set Start up position on destination image coordinate
  register int      _dstOffsetY = rPos.getY()*getLineSize(0);  //Set Start up position on destination image coordinate
  register int      _srcOffsetX = 0;
  register int      _srcOffsetY = 0;
  register u_int8_t _alpha      = 0;
  register int      _srcIncX    = (rImage.getFormat()==PIX_FMT_RGB24)?3:4;
  
  for ( register int y = 0; y < _srcRect.getHeight(); y++ )
  {
    _dstOffsetX = rPos.getX() << 2;
    _srcOffsetX = 0;
    
    for ( register int x = 0; x < _srcRect.getWidth(); x++ )
    {
      _dstNdx = _dstOffsetY + _dstOffsetX;
      _srcNdx = _srcOffsetY + _srcOffsetX;

      // Checking index to be valid.
      if ( ( _dstNdx >= 0 ) && ( _dstNdx < iDstBuffer ) )
      {
	_alpha = pDstBuffer[_dstNdx+3];
	
	if ( _alpha < 255 )
	{
	  pDstBuffer[_dstNdx+0] = ALPHA_BLEND(_alpha, pSrcBuffer[_srcNdx+0], pDstBuffer[_dstNdx+0], 0 );
	  pDstBuffer[_dstNdx+1] = ALPHA_BLEND(_alpha, pSrcBuffer[_srcNdx+1], pDstBuffer[_dstNdx+1], 0 );
	  pDstBuffer[_dstNdx+2] = ALPHA_BLEND(_alpha, pSrcBuffer[_srcNdx+2], pDstBuffer[_dstNdx+2], 0 );
	}
      }
      
      _dstOffsetX += 4;
      _srcOffsetX += _srcIncX;
    }
    
    _dstOffsetY += getLineSize(0);
    _srcOffsetY += rImage.getLineSize(0);
  }
  
  return eAVSucceded;
}

AVResult        CAVImage::blend( const CAVPoint& rPos, const CAVImage& rImage, const CAVImage& rMask, const CAVColor& cr )
{
  if ( m_eFormat != PIX_FMT_RGBA )
    return eAVUnsupportedOperation;
  
  if (
       ( rImage.getFormat() != PIX_FMT_RGB24 ) &&
       ( rImage.getFormat() != PIX_FMT_RGBA  )
     )
    return eAVUnsupportedInputFormat;
 
  if (
       ( this->getWidth()  != rMask.getWidth()  ) ||
       ( this->getHeight() != rMask.getHeight() ) ||
       ( this->getFormat() != rMask.getFormat() )
     )
    return eAVUnsupportedInputFormat;
  
  // @todo just process ovelapping rectangle.
  // rImage rect must overlap dest image
  
  int                  iDstBuffer = 0;
  register u_int8_t*   pDstBuffer = (u_int8_t*)getData( 0, &iDstBuffer );

  int                  iSrcBuffer = 0;
  register u_int8_t*   pSrcBuffer = (u_int8_t*)rImage.getData( 0, &iSrcBuffer );
  
  int                  iMaskBuffer = 0;
  register u_int8_t*   pMaskBuffer = (u_int8_t*)rMask.getData( 0, &iMaskBuffer );
  
  CAVRect  _dstRect = this->getRect();  // Calling methods on CAVRect is more efficient than calling 
                                        // methods on CAVImages.
  CAVRect  _srcRect = rImage.getRect(); // Source Rect in relarive coordinate system.
					
  int      _dstNdx; // will be used to keep destPos as array index.
  int      _srcNdx;
  
  register int      _dstOffsetX = rPos.getX() <<2;             //Set Start up position on destination image coordinate
  register int      _dstOffsetY = rPos.getY()*getLineSize(0);  //Set Start up position on destination image coordinate
  register int      _srcOffsetX = 0;
  register int      _srcOffsetY = 0;
  register u_int8_t _alpha      = 0;
  register int      _srcIncX    = (rImage.getFormat()==PIX_FMT_RGB24)?3:4;
  register u_int8_t _r          = 0;
  register u_int8_t _g          = 0;
  register u_int8_t _b          = 0;
  
  // Initialize local variables.
  cr.get( _r, _g, _b, _alpha );
  
  for ( register int y = 0; y < _srcRect.getHeight(); y++ )
  {
    // Using shift instead of multiplication
    // shift by 2 position can be identified as * 4
    _dstOffsetX = rPos.getX() << 2;
    _srcOffsetX = 0;
    
    for ( register int x = 0; x < _srcRect.getWidth(); x++ )
    {
      _dstNdx = _dstOffsetY + _dstOffsetX;
      _srcNdx = _srcOffsetY + _srcOffsetX;

      // Checking index to be valid.
      if ( ( _dstNdx >= 0 ) && ( _dstNdx < iDstBuffer ) )
      {
	_alpha = pDstBuffer[_dstNdx+3];
	
	if ( _alpha < 255 )
	{
	  if ( 
		(_r == pMaskBuffer[_dstNdx+0]) && 
		(_g == pMaskBuffer[_dstNdx+1]) && 
		(_b == pMaskBuffer[_dstNdx+2]) 
	    )
	  {	  
	    pDstBuffer[_dstNdx+0] = ALPHA_BLEND(_alpha, pSrcBuffer[_srcNdx+0], pDstBuffer[_dstNdx+0], 0 );
	    pDstBuffer[_dstNdx+1] = ALPHA_BLEND(_alpha, pSrcBuffer[_srcNdx+1], pDstBuffer[_dstNdx+1], 0 );
	    pDstBuffer[_dstNdx+2] = ALPHA_BLEND(_alpha, pSrcBuffer[_srcNdx+2], pDstBuffer[_dstNdx+2], 0 );
	  }
	}
      }
      _dstOffsetX += 4;
      _srcOffsetX += _srcIncX;
    }
    
    _dstOffsetY += getLineSize(0);
    _srcOffsetY += rImage.getLineSize(0);
  }
  
  return eAVSucceded;
}


}//namespace libavcpp

