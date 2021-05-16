

find_package(OpenImageIO)
add_library(openimageio INTERFACE)

if(NOT OpenImageIO_FOUND)

    #reference vfx-cmake
    # try to find header
    find_path( OpenImageIO_INCLUDE_DIR OpenImageIO/imageio.h
    $ENV{IMAGEIO_PATH}/include
    ${OpenImageIO_INSTALL_PATH}/include
    )

    # Support preference of static libs by adjusting CMAKE_FIND_LIBRARY_SUFFIXES
    if( OpenImageIO_USE_STATIC_LIBS )
    set( _oiio_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    if(WIN32)
        set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    else()
        set(CMAKE_FIND_LIBRARY_SUFFIXES .a )
    endif()
    endif()

    # try to find libs
    find_library( OpenImageIO_LIBRARY OpenImageIO
    $ENV{IMAGEIO_PATH}/lib
    $ENV{IMAGEIO_LIBRARY_PATH}
    ${OpenImageIO_INSTALL_PATH}/lib
    )

    # Restore the original find library ordering
    if( OpenImageIO_USE_STATIC_LIBS )
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${_oiio_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
    endif()

    # did we find everything?
    include( FindPackageHandleStandardArgs )
    FIND_PACKAGE_HANDLE_STANDARD_ARGS( "OpenImageIO" DEFAULT_MSG
    OpenImageIO_INCLUDE_DIR
    OpenImageIO_LIBRARY
    )
    target_link_libraries(openimageio INTERFACE OpenImageIO_LIBRARY)
    target_include_directories(openimageio INTERFACE OpenImageIO_INCLUDE_DIR)

else()

    target_link_libraries(openimageio INTERFACE OpenImageIO::OpenImageIO)

endif()
