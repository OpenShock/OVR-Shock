# Check if CMAKE_PREFIX_PATH contains Qt5 directory
if (NOT CMAKE_PREFIX_PATH MATCHES ".*Qt.*")
	message (FATAL_ERROR "Qt not found in CMAKE_PREFIX_PATH, aborting")
endif ()
if (NOT QT_QMAKE_EXECUTABLE MATCHES ".*qmake.*")
	message (FATAL_ERROR "QT_QMAKE_EXECUTABLE not found, aborting")
endif ()

find_package (
    Qt5
    COMPONENTS
        Core
        Gui
        OpenGL
        Widgets
        Multimedia
        MultimediaWidgets
        SerialPort
        Concurrent
        Network
    REQUIRED
)
if (NOT Qt5Core_FOUND)
    message (FATAL_ERROR "No Qt installation found, aborting")
endif ()

set (CMAKE_AUTOUIC ON)
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)

message ("Found Qt ${Qt5Core_VERSION}")

target_include_directories (${project} PRIVATE
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5OpenGL_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Multimedia_INCLUDE_DIRS}
    ${Qt5MultimediaWidgets_INCLUDE_DIRS}
    ${Qt5SerialPort_INCLUDE_DIRS}
    ${Qt5Concurrent_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
)

target_link_libraries (${project} PRIVATE
	${Qt5Core_LIBRARIES}
	${Qt5Gui_LIBRARIES}
	${Qt5OpenGL_LIBRARIES}
	${Qt5Widgets_LIBRARIES}
	${Qt5Multimedia_LIBRARIES}
	${Qt5MultimediaWidgets_LIBRARIES}
	${Qt5SerialPort_LIBRARIES}
	${Qt5Concurrent_LIBRARIES}
	${Qt5Network_LIBRARIES}
)