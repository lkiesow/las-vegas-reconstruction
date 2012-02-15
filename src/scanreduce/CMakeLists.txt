
set(REDUCE_SOURCES
    Options.cpp     
    Main.cpp
)

add_executable(reduce ${REDUCE_SOURCES})
set_target_properties(reduce PROPERTIES BINARY_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/bin)
target_link_libraries(reduce lssr)
target_link_libraries(reduce ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(reduce ${OpenMP_CXX_FLAGS})
target_link_libraries(reduce ${PCL_LIBRARIES})
if( ${NABO_FOUND} )
    target_link_libraries(reduce ${NABO_LIBRARIES})
endif( ${NABO_FOUND} )
