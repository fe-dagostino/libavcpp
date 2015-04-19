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

#ifndef AVFILTERGRAPH_H
#define AVFILTERGRAPH_H

#include "avcommon.h"
#include "FString.h"

USING_NAMESPACE_FED

extern "C"
{ 
#include <libavutil/pixfmt.h>
#include <libavutil/rational.h>
}

struct AVFrame;
struct AVFilterGraph;
struct AVCodecContext;
struct AVFilterContext;
struct AVFilterBufferRef;

namespace libavcpp
{
  
class CAVFilterGraph
{
public:
  /**
   * Empty constructor.
   */
  CAVFilterGraph( const FString& sFilters );
  /**
   * Destructor method. 
   * Automatically release all resources, so if decoder status
   * is still open it will be closed during destroy.
   */
  virtual ~CAVFilterGraph();
  
  /**
   * Check if current instance has been initialized or not.
   * @return true in case current object instance has been initialized 
   *         successful false otherwhise.
   */
  inline bool     isValid() const
  { return m_bValid; }
  
  /**
   * .
   */
  AVResult init( const AVCodecContext* pCodecCtx );
  
  /**
   * .
   */
  AVResult init(
		int iWidth, int iHeight,
		PixelFormat pixFormat,
		int iTimebaseNum, int iTimebaseDen,
		int iAspectRatioX, int iAspectRatioY
	      );  
  
  /**
   * Push frame into filter chain for processing.
   * @param pFrame      input frame to be processed by filter chain.
   * @param aspectRatio aspect ratio for video frame.
   */
  AVResult push( AVFrame* pFrame, const AVRational& aspectRatio );
  /**
   * Push frame into filter chain for processing.
   * @param pFrame      input frame to be processed by filter chain.
   * @param pCodecCtx   codec contex where to retrieve aspect ratio.
   */
  AVResult push( AVFrame* pFrame, const AVCodecContext* pCodecCtx );
  /**
   * Pop filtered buffer.
   */
  AVResult pop( bool &bMore, AVFilterBufferRef*& pAVFilterBufferRef );
  /**
   * Unference puffer. Must be called on each buffer returned from pop() method.
   * @param pAVFilterBufferRef buffer reverence to be mark as unreferenced so it will
   *                           be released.
   */
  AVResult unRef( AVFilterBufferRef*& pAVFilterBufferRef );

private:
  FString           m_sFilters;
  AVFilterGraph*    m_pFilterGraph;
  AVFilterContext*  m_pFilterCtxIN;
  AVFilterContext*  m_pFilterCtxOUT;
  bool              m_bValid;
};

} // namespace libavcpp

#endif // AVFILTERGRAPH_H
