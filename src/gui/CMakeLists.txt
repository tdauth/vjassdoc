set(VJASSDOC_GUI_MOC_H
	widget.hpp
)

set(VJASSDOC_GUI_RESOURCES
)

set(VJASSDOC_GUI_SRC
	application.cpp
	mainwindow.cpp
	widget.cpp
	main.cpp
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(..)

kde4_add_ui_files(VJASSDOC_GUI_SRC widget.ui)
QT4_AUTOMOC(widget.cpp)

kde4_add_executable(vjassdocgui ${VJASSDOC_GUI_SRC})
target_link_libraries(vjassdocgui vjassdoclib ${QT_LIBRARIES} ${KDE4_KIO_LIBS} ${KDE4_KUTILS_LIBS} ${KDE4_KPARTS_LIBS})

# install

install(TARGETS
	vjassdocgui
	DESTINATION bin
)
