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


#include "avapplication.h"

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
  
AVResult CAVApplication::initLibAVCPP()
{
    //Initialize libavcodec
    avcodec_register_all();
    
    //Initialize devices so it will be possible to open 
    //streaming like v4l or dev based streams
    avdevice_register_all();

    //Initialize the filter system. Register all builtin filters. 
    avfilter_register_all();  
    
    //Register all muxers, demuxers and protocols.
    av_register_all();  
    //Global initialization for network components.
    avformat_network_init();
    
    return eAVSucceded;
}

AVResult CAVApplication::deinitLibAVCPP()
{
    // Uninitialize the filter system. Unregister all filters.
    avfilter_uninit();
    
    // Undo the initialization done by avformat_network_init.
    avformat_network_deinit();
    
    return eAVSucceded;
}

}//namespace libavcpp
