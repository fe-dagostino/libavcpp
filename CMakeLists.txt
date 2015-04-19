
cmake_minimum_required (VERSION 2.6)
project(libav++ CXX)

include_directories( ./ )
include_directories( ./include/ )
include_directories( ${CMAKE_INSTALL_PREFIX}/include/ )
include_directories( ${CMAKE_INSTALL_PREFIX}/include/fedlibrary/include )
include_directories( ${CMAKE_INSTALL_PREFIX}/include/fedlibrary/template_include )

IF(NOT CMAKE_BUILD_TYPE )
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
ENDIF()

IF( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  set(  LIBAVCPP_VERSION
	-3.2.1-debug
    )
  set(  FEDLIB_VER
        -3.1.1-debug
     )
ELSE()
  set(  LIBAVCPP_VERSION
	-3.2.1
    )
  set(  FEDLIB_VER
        -3.1.1
     )
ENDIF()

set(  LIBAVCPP_BINARY_DIR 
      ${CMAKE_CURRENT_BINARY_DIR}
   )
   
set(  LIBAVCPP_INCLUDE 
        ./include/avfps.h
        ./include/avcolor.h 
        ./include/avpoint.h
        ./include/avrect.h
        ./include/avimage.h
        ./include/avframe.h
        ./include/avsample.h
        ./include/avcommon.h
        ./include/avfile.h
        ./include/avinputfile.h
        ./include/avoutputfile.h
        ./include/avdecoder.h
        ./include/avencoder.h
        ./include/avfiltergraph.h
        ./include/avapplication.h
)

set(  LIBAVCPP_SRC  
        ./src/avfps.cpp
        ./src/avcolor.cpp 
        ./src/avpoint.cpp
        ./src/avrect.cpp 
        ./src/avimage.cpp
        ./src/avframe.cpp
        ./src/avsample.cpp        
        ./src/avcommon.cpp
        ./src/avfile.cpp
        ./src/avinputfile.cpp
        ./src/avoutputfile.cpp
        ./src/avdecoder.cpp
        ./src/avencoder.cpp
        ./src/avfiltergraph.cpp
        ./src/avapplication.cpp
)

set(  LIBAVCPP_LIBRARIES
      pthread
      fedlibrary${FEDLIB_VER}
      avutil
      avcodec
      avfilter
      avformat
      avdevice
      swscale
)

IF( CMAKE_BUILD_TYPE STREQUAL "Debug" )
add_definitions(
                -ggdb3
)
ENDIF()

add_definitions(
                -std=c++0x
                -D__STDC_CONSTANT_MACROS
                -D_NO_FED_NAMESPACE
                -D_TRACING_EXCEPTION_RAISE
                -D_TRACING_EXCEPTION_CATCH
                -D_ENABLE_ERROR_INFO
                -D_ENABLE_LOG_INFO
                -D_ENABLE_RAW_INFO
                -D_ENABLE_METHOD_TRACING
                -D_ENABLE_VERBOSE_INFO 
                -D_ENABLE_ASSERTION_TRACING
               )

add_library   ( av++${LIBAVCPP_VERSION}  SHARED ${LIBAVCPP_SRC} )

target_link_libraries( av++${LIBAVCPP_VERSION}  ${LIBAVCPP_LIBRARIES} )


install( TARGETS av++${LIBAVCPP_VERSION}     LIBRARY DESTINATION lib )
install( FILES   ${LIBAVCPP_INCLUDE}                 DESTINATION include/libav++ )


# Build examples 
add_subdirectory( examples )



