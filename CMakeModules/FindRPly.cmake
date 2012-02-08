# Try to find RPly - ANSI C Library for PLY file format input and output
# This module will define the following variables:
#   RPLY_FOUND              -   indicates whether nabo was found on the system
#   RPLY_INCLUDE_DIRS       -   the directories for the nabo headerfiles
#   RPLY_LIBRARIES          -   the directory that contains the compiled library

find_path( RPLY_INCLUDE_DIR rply.h )
find_library( RPLY_LIBRARY NAMES rply librply )

set( RPLY_LIBRARIES ${RPLY_LIBRARY} )
set( RPLY_INCLUDE_DIRS ${RPLY_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RPly DEFAULT_MSG
    RPLY_LIBRARY RPLY_INCLUDE_DIR)
