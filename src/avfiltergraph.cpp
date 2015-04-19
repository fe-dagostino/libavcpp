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


#include "../include/avfiltergraph.h"

#include "FString.h"

USING_NAMESPACE_FED

extern "C"
{
#include <libavfilter/version.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/vsrc_buffer.h>
}


namespace libavcpp
{

CAVFilterGraph::CAVFilterGraph( const FString& sFilters )
 : m_sFilters( sFilters ), m_pFilterCtxIN( NULL ), m_pFilterCtxOUT( NULL ), m_bValid( false )
{
  m_pFilterGraph = avfilter_graph_alloc();
}
    
CAVFilterGraph::~CAVFilterGraph()
{
  if ( m_pFilterGraph != NULL )
  {
    avfilter_graph_free( &m_pFilterGraph );
    m_pFilterGraph = NULL;
  }
}

AVResult CAVFilterGraph::init( const AVCodecContext* pAVCodecCtx )
{
  if ( m_sFilters.IsEmpty() )
    return eAVInvalidParameters;
  if ( pAVCodecCtx == NULL )
    return eAVInvalidParameters;
    
  return init( 
	      pAVCodecCtx->width, pAVCodecCtx->height,
	      pAVCodecCtx->pix_fmt,
	      pAVCodecCtx->time_base.num, pAVCodecCtx->time_base.den,
	      pAVCodecCtx->sample_aspect_ratio.num, pAVCodecCtx->sample_aspect_ratio.den
	     );
}

AVResult CAVFilterGraph::init( 
			      int iWidth, int iHeight,
			      PixelFormat pixFormat,
			      int iTimebaseNum, int iTimebaseDen,
			      int iAspectRatioX, int iAspectRatioY
			     )
{
  if ( m_sFilters.IsEmpty() )
    return eAVInvalidParameters;
  
  int       iRetVal = 0;
  AVFilter* pFilterIN  = avfilter_get_by_name( "buffer" );
  if ( pFilterIN == NULL )
    return eAVFilterNotFound;
  
  AVFilter* pFilterOUT = avfilter_get_by_name( "nullsink" );
  if ( pFilterOUT == NULL )
    return eAVFilterNotFound;
    
  /* Buffer video source: the decoded frames from the codec will be inserted here. */
  FString _sBufferSource( 0, "%d:%d:%d:%d:%d:%d:%d", iWidth, iHeight, (int)pixFormat, iTimebaseNum, iTimebaseDen, iAspectRatioX, iAspectRatioY );

  iRetVal = avfilter_graph_create_filter( &m_pFilterCtxIN , pFilterIN, "input", (const char*)_sBufferSource, NULL, m_pFilterGraph ); 
  CHECK_AVRESULT( iRetVal )
    
  iRetVal = avfilter_graph_create_filter( &m_pFilterCtxOUT, pFilterOUT, "output", NULL, NULL, m_pFilterGraph ); 
  CHECK_AVRESULT( iRetVal )
  
  
  AVFilterInOut *outputs = (AVFilterInOut *)av_malloc(sizeof(AVFilterInOut));
  AVFilterInOut *inputs  = (AVFilterInOut *)av_malloc(sizeof(AVFilterInOut));
  
  /* Endpoints for the filter graph. */
  outputs->name       = av_strdup("in");
  outputs->filter_ctx = m_pFilterCtxIN;
  outputs->pad_idx    = 0;
  outputs->next       = NULL;
  inputs->name        = av_strdup("out");
  inputs->filter_ctx  = m_pFilterCtxOUT;
  inputs->pad_idx     = 0;
  inputs->next        = NULL;
  
  iRetVal = avfilter_graph_parse(m_pFilterGraph, (const char*)m_sFilters, inputs, outputs, NULL);
  CHECK_AVRESULT( iRetVal )

  iRetVal = avfilter_graph_config(m_pFilterGraph, NULL);
  CHECK_AVRESULT( iRetVal )
  
  m_bValid = true;
  
  return eAVSucceded;
}

AVResult CAVFilterGraph::push( AVFrame* pFrame, const AVRational& aspectRatio )
{
  int       iRetVal = 0;
  
  iRetVal = av_vsrc_buffer_add_frame( m_pFilterCtxIN, pFrame, pFrame->pts, aspectRatio );
  CHECK_AVRESULT( iRetVal )
  
  return eAVSucceded;
}

AVResult CAVFilterGraph::push( AVFrame* pFrame, const AVCodecContext* pCodecCtx )
{
  return push( pFrame, pCodecCtx->sample_aspect_ratio );
}

AVResult CAVFilterGraph::pop( bool &bMore, AVFilterBufferRef*& pAVFilterBufferRef )
{
  int           iRetVal  = 0;
  AVFilterLink* pOutLink = m_pFilterCtxOUT->inputs[0];
  
  // Check amount of available frames.  
  iRetVal  = avfilter_poll_frame( pOutLink );
  CHECK_AVRESULT( iRetVal )
  
  // Useful info for application level. 
  // Depends on filters one single input frame can produce one or more output frames.
  bMore              = (iRetVal > 1);
  pAVFilterBufferRef = NULL;
  
  if ( iRetVal )
  {
      iRetVal = avfilter_request_frame( pOutLink );  
      
      pAVFilterBufferRef = pOutLink->cur_buf;
  }
  
  return eAVSucceded;
}

AVResult  CAVFilterGraph::unRef( AVFilterBufferRef*& pAVFilterBufferRef )
{
  avfilter_unref_buffer(pAVFilterBufferRef);
  
  pAVFilterBufferRef = NULL;
  
  return eAVSucceded;
}

} // namespace libavcpp

