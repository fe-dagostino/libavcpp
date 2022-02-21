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

#ifndef VIDEO_FILTERGRAPH_H
#define VIDEO_FILTERGRAPH_H

#include "avcommon.h"
#include "iavfiltergraph.h"

#include "FString.h"

USING_NAMESPACE_FED

namespace libavcpp
{
  
class CVideoFilterGraph : public IAVFilterGraph
{
public:
  /**
   * Empty constructor.
   */
  CVideoFilterGraph( const FString& sFilters );
  /**
   * Destructor method. 
   * Automatically release all resources, so if decoder status
   * is still open it will be closed during destroy.
   */
  virtual ~CVideoFilterGraph();
  
  /**
   * .
   */
  AVResult init( const AVCodecContext* pCodecCtx );

  /**
   * Push frame into filter chain for processing.
   * @param pFrame      input frame to be processed by filter chain.
   */
  AVResult push( AVFrame* pFrame );
  
  /**
   * Pop filtered buffer.
   * Note: depending on filter/s applied one input frame can generate more
   *       than one output frame, so will be a good pratics to loop over 
   *       pop() until it return eAVSucceded . 
   */
  AVResult pop( AVFrame* pFrame );

  /**
   * Unference puffer. Must be called on each buffer returned from pop() method.
   * @param pFrame     buffer reference to be mark as unreferenced so it will
   *                   be released.
   */
  AVResult unRef( AVFrame*& pFrame );

private:
  /**
   * .
   */
  AVResult init(
		int iWidth, int iHeight, AVPixelFormat pixFormat,
		int iTimebaseNum, int iTimebaseDen,
		int iAspectRatioX, int iAspectRatioY
	      );  
  
private:
  FString           m_sFilters;
  AVFilterContext*  m_pFilterCtxIN;
  AVFilterContext*  m_pFilterCtxOUT;
  AVFilterContext*  m_pFilterCtxFTM;
};

} // namespace libavcpp

#endif // VIDEO_FILTERGRAPH_H
