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


#include "avfile.h"
#include "FMutex.h"

USING_NAMESPACE_FED

extern "C"
{
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
}


namespace libavcpp
{

static int volatile s_iInstanceCounter = 0;
static FMutex*      g_mtxAccess = new FMutex();

CAVFile::CAVFile()
{
  getMutex().EnterMutex();
  
  s_iInstanceCounter++;
  addInstance( s_iInstanceCounter );
  
  getMutex().LeaveMutex();
}

CAVFile::~CAVFile()
{
  getMutex().EnterMutex();
  
  s_iInstanceCounter--;
  delInstance( s_iInstanceCounter );

  getMutex().LeaveMutex();
}

void CAVFile::addInstance( int iCounter )
{
  if ( iCounter == 1 )
  {
     // add operations if required
  }
}

void CAVFile::delInstance( int iCounter )
{
  if ( iCounter == 0 )
  {
    // add operations if required
  }
}


FMutex&  CAVFile::getMutex()
{ return *g_mtxAccess; }


}//namespace libavcpp

