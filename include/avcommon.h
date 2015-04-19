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


#ifndef AVCOMMON_H
#define AVCOMMON_H

namespace libavcpp
{

enum AVResult
{
    eAVUndefined         =  0,
    eAVSucceded          =  1,
    eAVInvalidSequence   =  2,
    eAVBuffering         =  3,
    eAVBitstreamNotFound =  4,
    eAVDecoderNotFound   =  5,
    eAVDemuxesNotFound   =  6,
    eAVEncoderNotFound   =  7,
    eAVEndOfFile         =  8,
    eAVExit              =  9,
    eAVFilterNotFound    = 10,
    eAVInvalidData       = 11,
    eAVMuxerNotFound     = 12,
    eAVOptionNotFound    = 13,
    eAVPatchWelcome      = 14,
    eAVProtocolNotFound  = 15,
    eAVStreamNotFound    = 16,
    
    eAVInvalidParameters        = 100,
    eAVIOError                  = 101,
    eAVUnsupportedInputFormat   = 102,
    eAVUnsupportedOutputFormat  = 103,
    eAVTooLargeBufferRequired   = 104,
    eAVTooLongFilename          = 105,
    eAVNotEnoughMemory          = 106,
    eAVUnsupportedOperation     = 107
};

/*
 * Converts @param iResult return values in libav to AVResult
 */
AVResult getAVResult( int iResult );

/** 
 *  Used to check return value in libavcpp 
 */
#define CHECK_AVRESULT( code ){ \
	AVResult eResult = getAVResult( code );\
	if ( eResult != eAVSucceded )\
		return eResult;\
} 

}//namespace libavcpp


#endif // AVCOMMON_H

