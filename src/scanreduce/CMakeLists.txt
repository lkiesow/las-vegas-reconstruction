
set(REDUCE_SOURCES
    Options.cpp     
    Main.cpp
)

add_executable(scanreduce ${REDUCE_SOURCES})
set_target_properties(scanreduce PROPERTIES BINARY_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/bin)
target_link_libraries(scanreduce lssr)
target_link_libraries(scanreduce ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(scanreduce ${OpenMP_CXX_FLAGS})
target_link_libraries(scanreduce ${PCL_LIBRARIES})
if( ${NABO_FOUND} )
    target_link_libraries(scanreduce ${NABO_LIBRARIES})
endif( ${NABO_FOUND} )


INSTALL(PROGRAMS ${CMAKE_SOURCE_DIR}/bin/scanreduce
    DESTINATION bin)
