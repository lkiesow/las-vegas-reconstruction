
message( STATUS "Creating target convert…" )
add_executable( convertmodel convert.cpp )
target_link_libraries( convertmodel lssr )

if(PCL_FOUND)
    target_link_libraries( convertmodel ${PCL_LIBRARIES} )
endif(PCL_FOUND)


INSTALL(PROGRAMS ${CMAKE_SOURCE_DIR}/bin/convertmodel
    DESTINATION bin)
