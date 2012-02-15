
set(FILTER_SOURCES
    Options.cpp     
    Main.cpp
)

add_executable(scanfilter ${FILTER_SOURCES})

set_target_properties(scanfilter PROPERTIES BINARY_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/bin)
    
target_link_libraries(scanfilter lssr)
target_link_libraries(scanfilter ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY})
    
target_link_libraries(scanfilter ${PCL_LIBRARIES})


INSTALL(PROGRAMS ${CMAKE_SOURCE_DIR}/bin/scanfilter
    DESTINATION bin)
