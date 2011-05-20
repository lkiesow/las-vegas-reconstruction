set(LSSR_SOURCES
    io/PLYIO.cpp
    io/PLYTraits.cpp
    io/Progress.cpp
    io/Timestamp.cpp
)

add_library(lssr SHARED ${LSSR_SOURCES})
set_target_properties(lssr PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(lssr boost_system boost_program_options boost_filesystem boost_thread-mt)
target_link_libraries(lssr model3d)
target_link_libraries(lssr ${OPENGL_LIBRARY})
target_link_libraries(lssr ${OpenMP_CXX_FLAGS})