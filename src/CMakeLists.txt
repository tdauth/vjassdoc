set(VJASSDOC_SRC
	#objects
	object.cpp
	comment.cpp
	keyword.cpp
	key.cpp
	textmacro.cpp
	textmacroinstance.cpp
	type.cpp
	global.cpp
	local.cpp
	member.cpp
	parameter.cpp
	functioninterface.cpp
	function.cpp
	method.cpp
	call.cpp
	implementation.cpp
	hook.cpp
	interface.cpp
	struct.cpp
	module.cpp
	scope.cpp
	library.cpp
	sourcefile.cpp
	doccomment.cpp
	#program
	syntaxerror.cpp
	file.cpp
	parser.cpp
	compiler.cpp
	vjassdoc.cpp
	utilities.cpp
)

set(VJASSDOC_H
	object.hpp
	comment.hpp
	keyword.hpp
	key.hpp
	textmacro.hpp
	textmacroinstance.hpp
	type.hpp
	local.hpp
	global.hpp
	member.hpp
	functioninterface.hpp
	function.hpp
	method.hpp
	call.hpp
	parameter.hpp
	implementation.hpp
	hook.hpp
	interface.hpp
	struct.hpp
	module.hpp
	scope.hpp
	library.hpp
	sourcefile.hpp
	doccomment.hpp
	syntaxerror.hpp
	file.hpp
	parser.hpp
	compiler.hpp
	vjassdoc.hpp
	utilities.hpp
	internationalisation.hpp
)

# library
add_library(vjassdoclib SHARED ${VJASSDOC_SRC})
target_link_libraries(vjassdoclib ${GETTEXT_LIBRARIES} ${Boost_LIBRARIES})
if (SQLITE)
	target_link_libraries(vjassdoclib ${SQLITE_LIBRARY})
endif ()
set_target_properties(vjassdoclib PROPERTIES LINK_INTERFACE_LIBRARIES "")

# program
add_executable(vjassdoc main.cpp)
target_link_libraries(vjassdoc vjassdoclib)
set_target_properties(vjassdoc PROPERTIES LINK_INTERFACE_LIBRARIES "")

if (GUI)
	add_subdirectory(gui)
endif ()

#install

install(FILES
	${VJASSDOC_H}
	DESTINATION include/vjassdoc/
)

install(TARGETS
	vjassdoclib
	DESTINATION lib
)

install(TARGETS
	vjassdoc
	DESTINATION bin
)
