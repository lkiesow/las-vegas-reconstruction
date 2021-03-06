cmake_minimum_required(VERSION 2.8)
project(LSSR)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )
set( CMAKE_MODULE_PATH ${LSSR_SOURCE_DIR}/CMakeModules )

link_directories("${CMAKE_SOURCE_DIR}/lib")
link_directories("$ENV{HOME}/local/lib")

include_directories(${LSSR_SOURCE_DIR}/ext/Eigen)
include_directories(${LSSR_SOURCE_DIR}/ext/stann)
include_directories(${LSSR_SOURCE_DIR})
include_directories(${LSSR_SOURCE_DIR}/src/lib)


set( CMAKE_CXX_FLAGS         "-O3 -g" )
set( CMAKE_CXX_FLAGS_RELEASE "-O3 -msse3 -Wno-deprecated-declarations -Wno-unused -Wcpp" )
set( CMAKE_CXX_FLAGS_DEBUG   "-g -Wall" )

###############################################################
# EXTERNAL LIBRARIES
###############################################################


####
## Searching for NABO 
##############################

SET(NABO_INCLUDE_DIRS "")
SET(NABO_LIBRARIES "")
find_package(Nabo)
if(NABO_FOUND)
   include_directories(${NABO_INCLUDE_DIRS})
   link_directories(${NABO_LIBRARIES})
   add_definitions(-D_USE_NABO)
endif(NABO_FOUND)


####
## Searching for PCL
##############################

# We intentionally do this twice as there seems to be a bug in PCL 1.4 which
# leads to a non set PCL_VERSION variable if you call it only once.
find_package(PCL 1.0)
if ( "${PCL_VERSION}" STREQUAL "" )
    find_package(PCL 1.0)
endif ( "${PCL_VERSION}" STREQUAL "" )
if(PCL_FOUND)
    message(STATUS "Found PCL ${PCL_VERSION}")
	if ( "${PCL_VERSION}" VERSION_GREATER "1.0" )
        include_directories(${PCL_INCLUDE_DIRS})
        link_directories(${PCL_LIBRARY_DIRS})
        add_definitions(${PCL_DEFINITIONS})
        add_definitions(-D_USE_PCL_)
		if ( "${PCL_VERSION}" VERSION_GREATER "1.2" )
            add_definitions(-D_PCL_VERSION_GT_12_)
		else ( "${PCL_VERSION}" VERSION_GREATER "1.2" )
            add_definitions(-D_PCL_VERSION_12_)
		endif ( "${PCL_VERSION}" VERSION_GREATER "1.2" )
    else ( "${PCL_VERSION}" VERSION_GREATER "1.0" )
        message( "-- Your PCL version is to old.." )
        message( "-- PCL related stuff will be disabled." )
	endif ( "${PCL_VERSION}" VERSION_GREATER "1.0" )
else(PCL_FOUND)
        message( "-- No PCL found." )
        message( "-- PCL related stuff will be disabled." )
endif(PCL_FOUND)


####
## Searching for Boost
##############################

find_package( Boost 1.42
    COMPONENTS 
    program_options 
    filesystem system 
    thread 
    REQUIRED )
if(Boost_FOUND)
    link_directories(${BOOST_LIBRARY_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})
    add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
endif()


####
## Searching for OpenMP
##############################

find_package(OpenMP)
if(OpenMP_FOUND)
    link_directories(${OpenMP_LIBRARY_DIRS})
    include_directories(${OpenMP_INCLUDE_DIRS})
endif(OpenMP_FOUND)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OpenMP_CXX_FLAGS}" )
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${OpenMP_CXX_FLAGS}" )


####
## Searching for OpenGL
##############################

find_package(OpenGL REQUIRED)
if(OpenGL_FOUND)
    link_directories(${OpenGL_INCLUDE_DIRS})
    include_directories(${OpenGL_LIBRARY_DIRS})
endif(OpenGL_FOUND)

IF(APPLE)
    link_directories(/opt/local/lib)
    include_directories(/System/Library/Frameworks/GLUT.framework/Headers)
    include_directories(/System/Library/Frameworks/OpenGL.framework/Headers)
ENDIF(APPLE)

if(UNIX)
    FIND_PATH(OPENGL_INC gl.h /usr/include/GL)
    include_directories(${OPENGL_INC})
endif(UNIX)


####
## Searching for glut and it's additional runtime libraries.
##############################

find_package(GLUT REQUIRED)
if ( GLUT_FOUND )
    message( "-- Found OpenGL Utility Toolkit" )
else ( GLUT_FOUND )
    message( SEND_ERROR "-- could not find OpenGL Utility Toolkit" )
endif ( GLUT_FOUND )

if(LINUX)
if ( GLUT_Xmu_LIBRARY )
    message( "-- Found X.Org X11 libXmu/libXmuu runtime libraries" )
else ( GLUT_Xmu_LIBRARY )
    message( SEND_ERROR "-- Could not find X.Org X11 libXmu/libXmuu runtime libraries" )
endif ( GLUT_Xmu_LIBRARY )

if ( GLUT_Xi_LIBRARY )
    message( "-- Found X.Org X11 libXi runtime library" )
else ( GLUT_Xi_LIBRARY )
    message( SEND_ERROR "-- Could not find X.Org X11 libXi runtime library" )
endif ( GLUT_Xi_LIBRARY )
endif(LINUX)

##############################################################
# USED THIRD PARTY STUFF
##############################################################

# RPLY
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/ext/rply/ )
add_subdirectory(ext/rply)

# LASLIB
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/ext/laslib/inc )
add_subdirectory(ext/laslib)

# OBJ
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/ext/obj/ )
add_subdirectory(ext/obj)

##############################################################
# LIBRARIES
##############################################################

add_subdirectory(src/lib)

##############################################################
# BINARIES
##############################################################

add_subdirectory(src/reconstruct)
add_subdirectory(src/colorize)
add_subdirectory(src/scanreduce)
add_subdirectory(src/convert)
add_subdirectory(src/asciiconverter)

if(PCL_FOUND)
    add_subdirectory(src/scanfilter)
endif(PCL_FOUND)

##############################################################
# VIEWERS
##############################################################

set( QT_USE_QTOPENGL TRUE )
set( QT_USE_XML TRUE)
find_package( Qt4 )
if( QT4_FOUND )
    include_directories(${QT_INCLUDE_DIR})
    include_directories(${QT_QTGUI_INCLUDE_DIR})
    include_directories(${QT_QTOPENGL_INCLUDE_DIR})
    include_directories(${QT_QTCORE_INCLUDE_DIR})
    include_directories(${QT_QTXML_INCLUDE_DIR})

    # Try to find libQGLViewer
    find_library(QGLVIEWER_LIBRARY_RELEASE 
        NAMES qglviewer-qt4 qglviewer QGLViewer QGLViewer2 
        PATHS /usr/lib /usr/local/lib ENV QGLVIEWERROOT 
        ENV LD_LIBRARY_PATH ENV LIBRARY_PATH
        PATH_SUFFIXES QGLViewer QGLViewer/release)
    if(QGLVIEWER_LIBRARY_RELEASE)
        message(STATUS "Found libqglviewer")
        message(STATUS "Building qglviewer application")
        add_subdirectory(src/qglviewer)
    endif(QGLVIEWER_LIBRARY_RELEASE)

endif(QT4_FOUND)

################################################################
# DOCUMENTATION
################################################################

# Try to find doxygen
find_package( Doxygen )

# Doxygen found
if( NOT DOXYGEN_FOUND )
    message( WARNING "Could not find Doxygen. "
        "You will not be able to build the documentation." )
endif( NOT DOXYGEN_FOUND )

# GraphViz found
if( NOT DOXYGEN_DOT_FOUND )
    message( WARNING "Could not find GraphViz. "
        "You will not be able to build the documentation." )
endif( NOT DOXYGEN_DOT_FOUND )

if( DOXYGEN_FOUND AND DOXYGEN_DOT_FOUND )
    configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in 
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY )
    add_custom_target( doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM )
endif( DOXYGEN_FOUND AND DOXYGEN_DOT_FOUND )
