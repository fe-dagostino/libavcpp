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


#ifndef AVIMAGE_H
#define AVIMAGE_H

#include <stdio.h>
#include "avcommon.h"
#include "avrect.h"
#include "avcolor.h"

extern "C"
{ 
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavutil/imgutils.h>
}

struct SwsContext;
struct AVCodecContext;

namespace libavcpp
{

class CAVImage
{
public:
  /**
   * Empty constructor.
   */
  CAVImage();
  /**
   * Copy constructor.
   */
  CAVImage( const CAVImage& rImage );
  
  /**
   */
  ~CAVImage();
  
  /**
   *
   */
  AVResult       init( const AVFrame* pFrame, 
		       const AVCodecContext* pAVCodecCtx, 
		       int dstW, int dstH, 
		       AVPixelFormat dstFormat = AV_PIX_FMT_RGB24,
		       int flags = SWS_FAST_BILINEAR
		       );

  /**
   *
   */
  AVResult      init(
                      const CAVImage& rImage,
                      int dstW, int dstH,
                      AVPixelFormat dstFormat = AV_PIX_FMT_RGB24,
                      int flags = SWS_FAST_BILINEAR
                    );

  /**
   *
   */
  AVResult      init(
                      const AVFrame* pFrame,
                      int srcW, int srcH
                    );

  /**
   *
   */
  AVResult      init( const AVFrame* pFrame,
                      int srcW, int srcH,
                      int dstW, int dstH,
                      AVPixelFormat dstFormat = AV_PIX_FMT_RGB24,
                      int flags = SWS_FAST_BILINEAR );

  /**
   *  @param iPlane specify n-plane for the desidered buffer. 
   *                Value for this parameter must be between 0 and 3. 
   *  @param pSize if valid it will contains the size, in bytes,
   *               for the returned buffer.
   *  @return NULL if no image buffers is present and this means the
   *          object hasn't been initialized; A valid pointer otherwise.
   */
  const unsigned char*    getData( int iPlane, int* pSize ) const;
  /**
   * Non const overload for getData().
   */
  unsigned char*    	  getData( int iPlane, int* pSize );
  /**
   * Detach current allocated memory buffer setting internal variable 
   * members to NULL, allocated pointer will be in charge of caller
   * and should be released with free().
   */
  unsigned char*    	  detachData( int iPlane, int* pSize );
  
  /**
   */
  inline const AVFrame*   getFrame() const 
  { return m_pFrame; }
  
  /**
   * Return bounding rect in relative coordinate system.
   */
  CAVRect                 getRect() const;
  
  /**
   *  Return image Width in pixel.
   *  Return -1 if object is invalid or not inizialized.
   */
  int                     getWidth() const;
  
  /**
   *  Return image Height in pixel.
   *  Return -1 if object is invalid or not inizialized.
   */
  int                     getHeight() const;
  
  /**
   *  Return image Format.
   */
  enum AVPixelFormat      getFormat() const;
  
  /**
   *  Retrieve line size in byte.
   * 
   *  @param iPlane specify n-plane for the desidered line size. 
   *                Value for this parameter must be between 0 and 3. 
   *  @return  -1 if object is invalid or not inizialized.
   */
  inline int              getLineSize( int iPlane ) const
  {
    if ( m_pFrame == NULL )
      return -1;
  
    return m_pFrame->linesize[iPlane];
  }
  
  /***/
  inline bool             isValid() const
  { return (m_iSize > 0); }
  
  /**
   * Release resources and invalidate current instance.
   */
  void                    free();
  
  /**
   *  Convert an RGB image to PPM file.
   * 
   *  @param pFilename name for the ppm destination file.
   */
  AVResult                saveToPPM( const char* pFilename );


  /**
   *  Load specified file image. Most of the common formats are supported.
   *  More in details the method is able to load png, jpeg ..
   * 
   *  @param pFilename   source file name.
   *  @param iWidth      specify destination image width; if this parameter
   *                     will be -1 width will be the same as the original file.
   *  @param iHeight     specify destination image height; if this parameter
   *                     will be -1 height will be the same as the original file.
   *  @param eFormat     specify how to internaly store the image.
   */
  AVResult                load( 
                                const char*         pFilename,
                                int                 iWidth,
                                int                 iHeight,
                                enum AVPixelFormat  eFormat
                              );

  /**
   * Use alpha blending with current image and specified one.
   * 
   * @param rPos      Specify origin position on current image. Drawing of rImage
   *                  will start from that position.
   * @param rImage    Image to be blend on. 
   *                  
   * Note: current image must be in PIX_FMT_RGBA format.
   * Note: rImage must be in PIX_FMT_RGB24 or PIX_FMT_RGBA format .
   */
  AVResult                blend( const CAVPoint& rPos, const CAVImage& rImage );

  /**
   * Use alpha blending with current image and specified one.
   * 
   * @param rPos      Specify origin position on current image. Drawing of rImage
   *                  will start from that position.
   * @param rImage    Image to be blend on. 
   *                  
   * Note: current image must be in PIX_FMT_RGBA format.
   * Note: rImage must be in PIX_FMT_RGB24 or PIX_FMT_RGBA format.
   */
  AVResult                blend( const CAVPoint& rPos, const CAVImage& rImage, const CAVImage& rMask, const CAVColor& cr );
  
private:
  SwsContext*        m_pSwsContext;
  int                m_iSize;
  AVFrame*           m_pFrame;
  int                m_iWidth;
  int                m_iHeight;
  enum AVPixelFormat m_eFormat;
};

}//namespace libavcpp

#endif // AVIMAGE_H
