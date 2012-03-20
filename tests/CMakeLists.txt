include_directories(../src)

add_executable(autocompletion autocompletion.cpp)
target_link_libraries(autocompletion vjassdoclib)
set_target_properties(vjassdocgui PROPERTIES LINK_INTERFACE_LIBRARIES "")

install(TARGETS
	autocompletion
	DESTINATION bin
)