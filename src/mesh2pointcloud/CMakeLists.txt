
message( STATUS "Creating target mesh2pointcloud…" )
add_executable( mesh2pointcloud mesh2pointcloud.cpp )
target_link_libraries( mesh2pointcloud staticlssr )

if(PCL_FOUND)
    target_link_libraries( mesh2pointcloud ${PCL_LIBRARIES} )
endif(PCL_FOUND)
