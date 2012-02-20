cmake_minimum_required(VERSION 2.6.4)

project(vjassdoc)

# The version number.
set(vjassdoc_VERSION_MAJOR 0)
set(vjassdoc_VERSION_MINOR 4)
set(vjassdoc_VERSION "${vjassdoc_VERSION_MAJOR}.${vjassdoc_VERSION_MINOR}")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/vjassdocConfig.h.in"
	"${PROJECT_BINARY_DIR}/vjassdocConfig.h"
)

include_directories("${PROJECT_BINARY_DIR}")

option(DEBUG
	"Debug mode."
	OFF)
option(GUI
	"Creates a KDE SC based GUI."
	OFF)
option(SQLITE
	"Adds database support."
	OFF)
option(DOC
	"Generates API documentation as well."
	OFF)


find_package(Gettext REQUIRED)

if (GETTEXT_FOUND)
	include_directories(${GETTEXT_INCLUDE_DIR})
endif ()

# todo: require GNU C library for getopt
#find_package(GLIB2 REQUIRED)

# todo: require tokenizer component.
find_package(Boost REQUIRED)

if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
endif ()

# setup flags

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	add_definitions(-DDEBUG=1)
	message(STATUS "Build type is \"Debug\", using -DDEBUG.")
endif ()

if (GUI)
	find_package(Qt4 REQUIRED)
	find_package(KDE4 REQUIRED)

	if (QT4_FOUND AND KDE4_FOUND)
		include(${QT_USE_FILE})
		add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
		#add_definitions(-DQT_STL)
		link_directories(${KDE4_LIB_DIR})
		include_directories(${KDE4_INCLUDE_DIR} ${KDE4_INCLUDES} ${QT_INCLUDE_DIR})
	endif ()
endif ()

if (SQLITE)
	find_library(SQLITE_LIBRARY sqlite3 REQUIRED)
	find_file(SQLITE_HEADER sqlite3.h REQUIRED)

	if (SQLITE_LIBRARY AND SQLITE_HEADER)
		get_filename_component(SQLITE_INCLUDE_DIR ${SQLITE_HEADER}  PATH)
		message(STATUS "SQlite has been found (${SQLITE_LIBRARY}, ${SQLITE_INCLUDE_DIR}).")
		include_directories(${SQLITE_INCLUDE_DIR})
	endif ()
endif ()

add_definitions(-DLOCALE_DIR=\"${CMAKE_INSTALL_PREFIX}/share/locale\")

add_subdirectory(src)
add_subdirectory(locale)

if (DEBUG)
	add_subdirectory(tests)
endif ()
