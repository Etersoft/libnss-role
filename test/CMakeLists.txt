configure_file(test_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/test_config.h)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(TEST_SOURCES
	test_main.c
	test_parser.c
	test_paths.c)

add_mocked_test(main
	SOURCES ${TEST_SOURCES}
	COMPILE_OPTIONS -g3 -Wl,--wrap=librole_config_file -Wl,--rpath=${CMAKE_BINARY_DIR}
	LINK_LIBRARIES role pam pam_misc)

target_include_directories(test_main PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../include
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_BINARY_DIR}/include)

