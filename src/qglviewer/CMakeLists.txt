# Objects that use Qt's signals and slots

ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

set(QVIEWER_MOCS 
    app/ViewerApplication.h
    data/DataManager.h 
    viewers/Viewer.h
    viewers/ViewerManager.h
)

# Dynamically generated GUI object's from
# QtDesigner 
set(QVIEWER_UIS
    ui/FogDensityDialog.ui
    ui/InputCoordinateSystemDialog.ui
    ui/MainWindow.ui
    ui/SceneDockWidget.ui
)

# Main Sources
set(QVIEWER_SOURCES
    app/Main.cpp
    app/ViewerApplication.cpp
    data/DataCollector.cpp
    data/DataCollectorFactory.cpp
    data/DataManager.cpp
    data/Static3DDataCollector.cpp
    viewers/OrthoCamera.cpp
    viewers/PerspectiveViewer.cpp
    viewers/Viewer.cpp
    viewers/ViewerManager.cpp
    widgets/CustomTreeWidgetItem.cpp
)

set(QVIEWER_RESOURCES
    res/viewer.qrc
)

# Generate mocs and header files for qt objects
# and forms
qt_wrap_cpp(qviewer QVIEWER_MOC_SRC ${QVIEWER_MOCS})
qt_wrap_ui(qviewer QVIEWER_UI_HDRS QVIEWER_UI_SRC ${QVIEWER_UIS})
qt4_add_resources(QVIEWER_RES_SRC ${QVIEWER_RESOURCES})

add_definitions(${QT_DEFINITIONS})

# Add current binary dir to includes to find
# the generated header files
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Create a new targer
add_executable(qviewer ${QVIEWER_SOURCES} ${QVIEWER_MOC_SRC} ${QVIEWER_UI_HDRS} ${QVIEWER_RES_SRC})

# Add needed libraries
target_link_libraries(qviewer ${QT_QTCORE_LIBRARIES})
target_link_libraries(qviewer ${QT_QTGUI_LIBRARIES})
target_link_libraries(qviewer ${QT_QTOPENGL_LIBRARIES})
target_link_libraries(qviewer model3d)
target_link_libraries(qviewer ${OPENGL_LIBRARY})
target_link_libraries(qviewer ${GLUT_LIBRARIES})
target_link_libraries(qviewer qglviewer-qt4)
target_link_libraries(qviewer boost_system boost_program_options boost_filesystem boost_thread)