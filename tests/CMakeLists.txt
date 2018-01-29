include_directories(../src)

add_executable(autocompletion autocompletion.cpp)
target_link_libraries(autocompletion vjassdoclib)
add_test(NAME Autocompletion COMMAND autocompletion)

install(TARGETS
	autocompletion
	DESTINATION bin
)
