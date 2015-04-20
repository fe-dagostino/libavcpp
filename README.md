# libav++
C++ Wrapper and Facilities to simplify libav usage

Would like to provide and easy and fast way to access and use the powerful libav. 

Here a list with all dependecies and their use

 - [**LibAV**](https://www.libav.org/) Libav provides cross-platform tools and libraries to convert, manipulate and stream a wide range of multimedia formats and protocols. Note that libav make use of many other libraries that add protocols and functionalities to libav itself, make sure to refers to LibAV website for a complete view of all protocols and formats supported by this library.
 - [**FEDLibrary**](http://fedlibrary.sourceforge.net/) used to keep code portable between linux and windows OSs
 
# Build instructions

Please refer to the Building pages in the [wiki](https://github.com/fe-dagostino/libavcpp/wiki).

# Examples

 - decoder.cpp     this is an easy example that show how to initialize libav++ and how implement IAVDecoderEvents
                   interface in order to handle each single frame. This application just take as input and video file,
                   then open the stream and read all frames until the end.
 - img2ppm.cpp     the program is able to convert a generic input file (image or video) to a ppm format.
                   img2ppm demostrates using of libav++ implementing the IAVDecoderEvents event interface and how to
                   use CAVImage for rescaling.
 - encoder.cpp     this program provide an example on how to encode a video file using and instance of CAVEncoder class. 
                   Encoded file will be a result of blending input frames coming from a video file together with an image
                   file used as background, in order to cover the scope background image file should have transparent areas
                   where the video will be visible. Output file will be an mp4 file.
 - ogl_cube.cpp    one more example using integration of libav++ with an opengl context. In this example an input stream
                   will be used to create dynamic textures and to mapping it to a cube.

                   ![alt tag](https://raw.github.com/fe-dagostino/libavcpp/master/examples/res/ogl_cube.png)
                   
                   Orginal code for opengl cube has been taken from http://nehe.gamedev.net/ and modified on order 
                   to replace static textures with a dynamic one.
 
