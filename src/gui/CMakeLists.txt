find_package(Qt4 REQUIRED)
find_package(KDE4 REQUIRED)

if (QT4_FOUND AND KDE4_FOUND)
	include(${QT_USE_FILE})
	add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
	add_definitions(-DQT_STL)
	link_directories(${KDE4_LIB_DIR})
	include_directories(${KDE4_INCLUDE_DIR} ${KDE4_INCLUDES} ${QT_INCLUDE_DIR})
endif ()

set(VJASSDOC_GUI_MOC_H
	widget.hpp
)

set(VJASSDOC_GUI_RESOURCES
)

set(VJASSDOC_GUI_SRC
	mainwindow.cpp
	widget.cpp
	main.cpp
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(..)

kde4_add_ui_files(VJASSDOC_GUI_SRC widget.ui)
QT4_AUTOMOC(widget.cpp)

kde4_add_executable(vjassdocgui ${VJASSDOC_GUI_SRC})
set_target_properties(vjassdocgui PROPERTIES LINK_INTERFACE_LIBRARIES "")
target_link_libraries(vjassdocgui vjassdoclib ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS})

# install

install(TARGETS
	vjassdocgui
	DESTINATION bin
)
