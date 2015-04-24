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

#ifndef IAVFILTERGRAPH_H
#define IAVFILTERGRAPH_H

#include "avcommon.h"
#include "FString.h"

USING_NAMESPACE_FED

extern "C"
{ 
#include <libavutil/pixfmt.h>
#include <libavcodec/avcodec.h>
}

struct AVFrame;
struct AVFilterGraph;
struct AVCodecContext;
struct AVFilterContext;

namespace libavcpp
{
  
class IAVFilterGraph
{
public:
  /**
   * Empty constructor.
   */
  IAVFilterGraph( AVMediaType mt );
  /**
   * Destructor method. 
   * Automatically release all resources, so if decoder status
   * is still open it will be closed during destroy.
   */
  virtual ~IAVFilterGraph();
  
  /**
   * Check if current instance has been initialized or not.
   * @return true in case current object instance has been initialized 
   *         successful false otherwhise.
   */
  inline bool            isValid() const
  { return m_bValid; }
  
  /***/
  inline AVMediaType     getMediaType() const
  { return m_avMediaType; }
  
  /**
   * .
   */
  virtual AVResult init( const AVCodecContext* pCodecCtx ) = 0;
  
  /**
   * Push frame into filter chain for processing.
   * @param pFrame      input frame to be processed by filter chain.
   */
  virtual AVResult push( AVFrame* pFrame ) = 0;
  
  /**
   * Pop filtered buffer.
   * Note: depending on filter/s applied one input frame can generate more
   *       than one output frame, so will be a good pratics to loop over 
   *       pop() until it return eAVSucceded . 
   */
  virtual AVResult pop( AVFrame* pFrame ) = 0;

  /**
   * Unference puffer. Must be called on each buffer returned from pop() method.
   * @param pFrame     buffer reference to be mark as unreferenced so it will
   *                   be released.
   */
  virtual AVResult unRef( AVFrame*& pFrame ) = 0;

protected:
  AVFilterGraph*    m_pFilterGraph;
  bool              m_bValid;
  
private:
  AVMediaType       m_avMediaType;
  
};

} // namespace libavcpp

#endif // IAVFILTERGRAPH_H
