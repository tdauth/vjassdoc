include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")

configure_file(function.j function.j COPYONLY)
configure_file(native.j native.j COPYONLY)
configure_file(type.j type.j COPYONLY)
add_executable(parser_test parser.cpp)
target_link_libraries(parser_test vjassdoclib ${Boost_LIBRARIES})
add_test(NAME ParserTest COMMAND parser_test)
