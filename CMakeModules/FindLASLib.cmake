# Try to find RPly - ANSI C Library for PLY file format input and output
# This module will define the following variables:
#   LASLIB_FOUND              -   indicates whether nabo was found on the system
#   LASLIB_INCLUDE_DIRS       -   the directories for the nabo headerfiles
#   LASLIB_LIBRARIES          -   the directory that contains the compiled library

find_path( LASLIB_INCLUDE_DIR lasreader.hpp laswriter.hpp )
find_library( LASLIB_LIBRARY NAMES laslib liblaslib )

set( LASLIB_LIBRARIES ${LASLIB_LIBRARY} )
set( LASLIB_INCLUDE_DIRS ${LASLIB_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LASLib DEFAULT_MSG
    LASLIB_LIBRARY LASLIB_INCLUDE_DIR)
