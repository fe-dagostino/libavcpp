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


#include "../include/audiofiltergraph.h"

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

CAudioFilterGraph::CAudioFilterGraph( const FString& sFilters )
 : IAVFilterGraph( AVMEDIA_TYPE_AUDIO ),  m_sFilters( sFilters ), 
   m_pFilterCtxIN( NULL ), m_pFilterCtxOUT( NULL ), m_pFilterCtxFTM( NULL )
{
  
}
    
CAudioFilterGraph::~CAudioFilterGraph()
{
}

AVResult CAudioFilterGraph::init( const AVCodecContext* pAVCodecCtx )
{
  if ( m_sFilters.IsEmpty() )
    return eAVInvalidParameters;
  if ( pAVCodecCtx == NULL )
    return eAVInvalidParameters;
    
  if ( pAVCodecCtx->codec_type != AVMEDIA_TYPE_AUDIO )
    return eAVInvalidParameters;
  
  if ( m_sFilters.IsEmpty() )
    return eAVInvalidParameters;

  int       iRetVal = 0;
  
  AVFilter* pFilterIN  = avfilter_get_by_name( "abuffer" );
  if ( pFilterIN == NULL )
    return eAVFilterNotFound;

  
  
  
  
  
  iRetVal = avfilter_graph_config(m_pFilterGraph, NULL);
  CHECK_AVRESULT( iRetVal )
  
  m_bValid = true;
  
  return eAVSucceded;
}

AVResult CAudioFilterGraph::push( AVFrame* pFrame )
{
  int       iRetVal = 0;
  
  iRetVal = av_buffersrc_add_frame( m_pFilterCtxIN, pFrame );
  CHECK_AVRESULT( iRetVal )
  
  return eAVSucceded;
}

AVResult CAudioFilterGraph::pop( AVFrame* pFrame )
{
  int       iRetVal = 0;
  
  iRetVal = av_buffersink_get_frame( m_pFilterCtxOUT, pFrame);
  CHECK_AVRESULT( iRetVal )
  
  return eAVSucceded;
}

AVResult  CAudioFilterGraph::unRef( AVFrame*& pFrame )
{
  av_frame_unref(pFrame);
  
  pFrame = NULL;
  
  return eAVSucceded;
}

} // namespace libavcpp

