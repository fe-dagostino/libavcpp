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


#ifndef AVENCODER_H
#define AVENCODER_H

#include "avcommon.h"
#include "avoutputfile.h"
#include "avframe.h"


struct AVFifoBuffer;

namespace libavcpp
{

class CAVOption
{
};


class CAVEncoder
{

public:
    /**
     */
    CAVEncoder( );
    /**
     */
    virtual ~CAVEncoder();

    /**
     */
    virtual AVResult open( const char* pFilename,
                           unsigned int uiAVFlags,
                           int dstW, int dstH,
                           AVPixelFormat  dstPixFtm,
                           int dstFps, int dstGOP,
                           int dstBitRate,
                           AVCodecID dstVideoCodec,
                           int dstVideoProfile
                         );

    /**
     * Retrieve output video width.
     * @return -1 if called before open or if no video stream has been defined; a valid
     *         positive value in case of success.
     */
    virtual int getVideoWidth() const;
    /**
     * Retrieve output video height.  
     * @return -1 if called before open or if no video stream has been defined; a valid
     *         positive value in case of success.
     */
    virtual int getVideoHeight() const;
			 
    /**
     * This method will write an Audio/Video frame in the stream.
     * Note: that frame format must have the same format and the same size specified
     *       opening the encoder. If you try to write a Frame with different parameters
     *       return value will be eAVInvalidParameters. So make sure to convert original
     *       image accordingly with encoder.
     *
     * @return in case of success eAVSucceded will be returned.
     * @see CAVImage
     */
    virtual AVResult write( const CAVFrame*  pAVFrame, unsigned int uiFlags );
    virtual AVResult write( const CAVImage*  pAVFrame, unsigned int uiFlags );
    virtual AVResult write( const CAVSample* pAVFrame, unsigned int uiFlags );
    
    /**
     */
    virtual AVResult flush( unsigned int uiFlags );
    /**
     */
    virtual AVResult close();
    
    /**
    *  Check if encoder is currently open or not.
    *  @return true if encoder is open, false otherwise
    */
    virtual bool     isOpened() const;

private:
    CAVOutputFile*   m_pAVOutputFile;
};

}//namespace libavcpp


#endif // AVENCODER_H
