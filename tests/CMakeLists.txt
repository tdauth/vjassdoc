add_executable(autocompletion autocompletion.cpp)
target_link_libraries(autocompletion vjassdoclib)

install(TARGETS
	autocompletion
	DESTINATION bin
)