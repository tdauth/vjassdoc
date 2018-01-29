cmake_minimum_required(VERSION 3.0)

project(vjassdoc)

# The version number.
set(vjassdoc_VERSION_MAJOR 0)
set(vjassdoc_VERSION_MINOR 4)
set(vjassdoc_VERSION "${vjassdoc_VERSION_MAJOR}.${vjassdoc_VERSION_MINOR}")

enable_testing()

include(CTest)

# configure a header file to pass some of the CMake settings
# to the source codeB
configure_file (
	"${PROJECT_SOURCE_DIR}/vjassdocConfig.h.in"
	"${PROJECT_BINARY_DIR}/vjassdocConfig.h"
)
install(FILES "${PROJECT_BINARY_DIR}/vjassdocConfig.h" DESTINATION include/vjassdoc/)

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

find_package(Boost COMPONENTS filesystem unit_test_framework REQUIRED)
add_definitions(-DBOOST_TEST_DYN_LINK)

if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
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

# required for locale files - gettext
add_definitions(-DLOCALE_DIR=\"${CMAKE_INSTALL_PREFIX}/share/locale\")

add_subdirectory(src)
add_subdirectory(locale)
