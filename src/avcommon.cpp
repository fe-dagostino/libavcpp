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


#include "avcommon.h"

extern "C"
{
#include <libavutil/avutil.h>
}


namespace libavcpp
{

AVResult getAVResult( int iResult )
{
   if ( iResult >= 0 )
      return eAVSucceded;

   switch ( iResult )
   {
      case AVERROR_BSF_NOT_FOUND     : return eAVBitstreamNotFound; ///< Bitstream filter not found
      case AVERROR_DECODER_NOT_FOUND : return eAVDecoderNotFound;   ///< Decoder not found
      case AVERROR_DEMUXER_NOT_FOUND : return eAVDemuxesNotFound;   ///< Demuxer not found
      case AVERROR_ENCODER_NOT_FOUND : return eAVEncoderNotFound;   ///< Encoder not found
      case AVERROR_EOF               : return eAVEndOfFile;         ///< End of file
      case AVERROR_EXIT              : return eAVExit;              ///< Immediate exit was requested; the called function should not be restarted
      case AVERROR_FILTER_NOT_FOUND  : return eAVFilterNotFound;    ///< Filter not found
      case AVERROR_INVALIDDATA       : return eAVInvalidData;       ///< Invalid data found when processing input
      case AVERROR_MUXER_NOT_FOUND   : return eAVMuxerNotFound;     ///< Muxer not found
      case AVERROR_OPTION_NOT_FOUND  : return eAVOptionNotFound;    ///< Option not found
      case AVERROR_PATCHWELCOME      : return eAVPatchWelcome;      ///< Not yet implemented in Libav, patches welcome
      case AVERROR_PROTOCOL_NOT_FOUND: return eAVProtocolNotFound;  ///< Protocol not found
      case AVERROR_STREAM_NOT_FOUND  : return eAVStreamNotFound;    ///< Stream not found
   } 

   return eAVUndefined;
}

}//namespace libavcpp


