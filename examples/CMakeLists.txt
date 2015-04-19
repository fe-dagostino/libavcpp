
cmake_minimum_required (VERSION 2.6)
project(examples CXX)

include_directories( ../ )

# Add the temporary output directories to the library path to make sure the
# libav++ library can be found, even if it is not installed system-wide yet.
LINK_DIRECTORIES( ${LIBAVCPP_BINARY_DIR} )

set(  DECODER_LIBRARIES
      av++${LIBAVCPP_VERSION}
)

set(  ENCODER_LIBRARIES
      av++${LIBAVCPP_VERSION}
      SDL2
)

set(  RENDER_LIBRARIES
      av++${LIBAVCPP_VERSION}
      X11 
      glut
      GL
      GLU
)

set(  RECORDER_LIBRARIES
      av++${LIBAVCPP_VERSION}
)

set(  IMG2PPM_LIBRARIES
      av++${LIBAVCPP_VERSION}
)


#add_executable( libavcppd                           ${LIBAVCPP_SRC} main.cpp )
#add_executable( decoder                             decoder.cpp ) 
#add_executable( encoder                             encoder.cpp )
#add_executable( encoder2                            encoder2.cpp )
#add_executable( render                              render.cpp  )
add_executable( img2ppm                              img2ppm.cpp )


#target_link_libraries( libavcppd                    ${LIBAVCPP_LIBRARIES} )
#target_link_libraries( decoder                      ${DECODER_LIBRARIES}  )
#target_link_libraries( encoder                      ${ENCODER_LIBRARIES}  )
#target_link_libraries( encoder2                     ${ENCODER_LIBRARIES}  )
#target_link_libraries( render                       ${RENDER_LIBRARIES}   )
target_link_libraries( img2ppm                       ${IMG2PPM_LIBRARIES}   )

