
cmake_minimum_required (VERSION 2.6)
project(examples CXX)

include_directories( ../ )

# Add the temporary output directories to the library path to make sure the
# libav++ library can be found, even if it is not installed system-wide yet.
LINK_DIRECTORIES( ${LIBAVCPP_BINARY_DIR} )


set(  DECODER_LIBRARIES
      av++${LIBAVCPP_VERSION}
)

set(  IMG2PPM_LIBRARIES
      av++${LIBAVCPP_VERSION}
)

set(  ENCODER_LIBRARIES
      av++${LIBAVCPP_VERSION}
)

set(  OGL_CUBE_LIBRARIES
      av++${LIBAVCPP_VERSION}
      X11 
      glut
      GL
      GLU
)


add_executable( decoder                              decoder.cpp  ) 
add_executable( img2ppm                              img2ppm.cpp  )
add_executable( encoder                              encoder.cpp  )
add_executable( ogl_cube                             ogl_cube.cpp )


target_link_libraries( decoder                       ${DECODER_LIBRARIES}   )
target_link_libraries( img2ppm                       ${IMG2PPM_LIBRARIES}   )
target_link_libraries( encoder                       ${ENCODER_LIBRARIES}   )
target_link_libraries( ogl_cube                      ${OGL_CUBE_LIBRARIES}  )

