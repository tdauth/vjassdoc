find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
find_package(KDE4 REQUIRED)

if (QT4_FOUND AND KDE4_FOUND)
	include(${QT_USE_FILE})
	add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} ${KDE4_ENABLE_EXCEPTIONS})
	#add_definitions(-DQT_STL)
	link_directories(${KDE4_LIB_DIR})
	include_directories(${KDE4_INCLUDE_DIR} ${KDE4_INCLUDES} ${QT_INCLUDE_DIR})
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -g -rdynamic") # TODO workaround
endif ()

set(VJASSDOC_GUI_SRC
	mainwindow.cpp
	widget.cpp
	main.cpp
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(..)

kde4_add_ui_files(VJASSDOC_GUI_UI_H widget.ui)

kde4_add_executable(vjassdocgui ${VJASSDOC_GUI_SRC} ${VJASSDOC_GUI_UI_H})
target_link_libraries(vjassdocgui vjassdoclib ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS})
set_target_properties(vjassdocgui PROPERTIES LINK_INTERFACE_LIBRARIES "")

# install

install(TARGETS
	vjassdocgui
	DESTINATION bin
)
