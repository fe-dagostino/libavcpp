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


#include "../include/videofiltergraph.h"

#include "FString.h"

USING_NAMESPACE_FED

extern "C"
{
#include <libavfilter/version.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
}


namespace libavcpp
{

CVideoFilterGraph::CVideoFilterGraph( const FString& sFilters )
 : IAVFilterGraph( AVMEDIA_TYPE_VIDEO ),  m_sFilters( sFilters ), 
   m_pFilterCtxIN( NULL ), m_pFilterCtxOUT( NULL ), m_pFilterCtxFTM( NULL )
{
  
}
    
CVideoFilterGraph::~CVideoFilterGraph()
{
}

AVResult CVideoFilterGraph::init( const AVCodecContext* pAVCodecCtx )
{
  if ( m_sFilters.IsEmpty() )
    return eAVInvalidParameters;
  if ( pAVCodecCtx == NULL )
    return eAVInvalidParameters;

  if ( pAVCodecCtx->codec_type != AVMEDIA_TYPE_VIDEO )
    return eAVInvalidParameters;

  return init( 
                    pAVCodecCtx->width, pAVCodecCtx->height,
                    pAVCodecCtx->pix_fmt,
                    pAVCodecCtx->time_base.num, pAVCodecCtx->time_base.den,
                    pAVCodecCtx->sample_aspect_ratio.num, pAVCodecCtx->sample_aspect_ratio.den
                 );
}

AVResult CVideoFilterGraph::init( 
                                    int iWidth, int iHeight,
                                    AVPixelFormat pixFormat,
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
  
  AVFilter* pFilterOUT = avfilter_get_by_name( "buffersink" );
  if ( pFilterOUT == NULL )
    return eAVFilterNotFound;
    
  /* Buffer video source: the decoded frames from the codec will be inserted here. */
  FString _sBufferSource( 0, "%d:%d:%d:%d:%d:%d:%d", iWidth, iHeight, (int)pixFormat, iTimebaseNum, iTimebaseDen, iAspectRatioX, iAspectRatioY );

  iRetVal = avfilter_graph_create_filter( &m_pFilterCtxIN , pFilterIN, "input", (const char*)_sBufferSource, NULL, m_pFilterGraph ); 
  CHECK_AVRESULT( iRetVal )
    
  iRetVal = avfilter_graph_create_filter( &m_pFilterCtxOUT, pFilterOUT, "output", NULL, NULL, m_pFilterGraph ); 
  CHECK_AVRESULT( iRetVal )
  
  iRetVal = avfilter_graph_create_filter( &m_pFilterCtxFTM, avfilter_get_by_name("format"), "format", "yuv420p", NULL, m_pFilterGraph);
  CHECK_AVRESULT( iRetVal )
  
  iRetVal = avfilter_link(m_pFilterCtxFTM, 0, m_pFilterCtxOUT, 0);
  CHECK_AVRESULT( iRetVal )
  
  if (m_sFilters.IsEmpty())
  {
    iRetVal =  avfilter_link(m_pFilterCtxIN, 0, m_pFilterCtxFTM, 0);
    CHECK_AVRESULT( iRetVal )
  }
  else
  {
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
  
    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = m_pFilterCtxIN;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;
    
    inputs->name        = av_strdup("out");
    inputs->filter_ctx  = m_pFilterCtxFTM;
    inputs->pad_idx     = 0;
    inputs->next        = NULL;
    
    iRetVal = avfilter_graph_parse(m_pFilterGraph, (const char*)m_sFilters, inputs, outputs, NULL);
    CHECK_AVRESULT( iRetVal )
  } 

  iRetVal = avfilter_graph_config(m_pFilterGraph, NULL);
  CHECK_AVRESULT( iRetVal )
  
  m_bValid = true;
  
  return eAVSucceded;
}

AVResult CVideoFilterGraph::push( AVFrame* pFrame )
{
  int       iRetVal = 0;
  
  iRetVal = av_buffersrc_add_frame( m_pFilterCtxIN, pFrame );
  CHECK_AVRESULT( iRetVal )
  
  return eAVSucceded;
}

AVResult CVideoFilterGraph::pop( AVFrame* pFrame )
{
  int       iRetVal = 0;
  
  iRetVal = av_buffersink_get_frame( m_pFilterCtxOUT, pFrame);
  CHECK_AVRESULT( iRetVal )
  
  return eAVSucceded;
}

AVResult  CVideoFilterGraph::unRef( AVFrame*& pFrame )
{
  av_frame_unref(pFrame);
  
  pFrame = NULL;
  
  return eAVSucceded;
}

} // namespace libavcpp

