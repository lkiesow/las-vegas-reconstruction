set(LSSR_SOURCES
    io/BaseIO.cpp
    io/ModelFactory.cpp
    io/PLYIO.cpp
    io/AsciiIO.cpp
    io/UosIO.cpp
    io/ObjIO.cpp	
    io/LasIO.cpp
    io/PCDIO.cpp
    io/Progress.cpp
    io/Timestamp.cpp
    io/MeshBuffer.cpp
    io/PointBuffer.cpp
    io/GridIO.cpp
    display/CoordinateAxes.cpp
    display/Renderable.cpp
    display/PointCloud.cpp
    display/MultiPointCloud.cpp
    display/StaticMesh.cpp
    display/ColorMap.cpp
    display/GroundPlane.cpp
    display/Grid.cpp
    reconstruction/PCLFiltering.cpp
)


add_library(lssr ${LSSR_SOURCES})
message(STATUS "Building static library")

set_target_properties(lssr PROPERTIES LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/lib)

target_link_libraries(lssr ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_FILESYSTEM_LIBRARY}
    ${OpenMP_CXX_FLAGS} ${OPENGL_LIBRARY} )
target_link_libraries(lssr laslib)
target_link_libraries(lssr rply)
