cmake_minimum_required(VERSION 2.8)

PROJECT(rply)


ADD_LIBRARY(rply rply.c)
set_target_properties(rply PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)


# Install target for rply
if ( BUILD_SHARED_LIBS )
    # Set library dir.
    if ( "${LIB_INSTALL_DIR}" STREQUAL "" )
        set( LIB_INSTALL_DIR "lib" )
    endif ( "${LIB_INSTALL_DIR}" STREQUAL "" )

    # Set install target.
    INSTALL( TARGETS rply
        LIBRARY DESTINATION ${LIB_INSTALL_DIR} )
endif ( BUILD_SHARED_LIBS )
