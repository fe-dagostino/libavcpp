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


#include "iavfiltergraph.h"

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

IAVFilterGraph::IAVFilterGraph( AVMediaType mt )
 : m_avMediaType( mt ), m_bValid( false )
{
  m_pFilterGraph = avfilter_graph_alloc();
}
    
IAVFilterGraph::~IAVFilterGraph()
{
  if ( m_pFilterGraph != NULL )
  {
    avfilter_graph_free( &m_pFilterGraph );
    m_pFilterGraph = NULL;
  }
}

} // namespace libavcpp

