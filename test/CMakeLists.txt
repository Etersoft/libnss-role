set(WRAPPER_PASSWD "${CMAKE_CURRENT_BINARY_DIR}/data/passwd")
set(WRAPPER_GROUP "${CMAKE_CURRENT_BINARY_DIR}/data/group")
set(WRAPPER_HOSTS "${CMAKE_CURRENT_BINARY_DIR}/data/hosts")

configure_file(test_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/test_config.h)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(TEST_SOURCES
	test_main.c
	test_parser.c
	test_paths.c)

add_mocked_test(main
	SOURCES ${TEST_SOURCES}
	COMPILE_OPTIONS -g3 -Wl,--wrap=librole_config_file
	LINK_LIBRARIES role pam pam_misc)

target_include_directories(test_main PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../include
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_BINARY_DIR}/include)

add_custom_target(all_tests ALL
	DEPENDS test_main)

add_custom_command(TARGET all_tests
	COMMENT "Run tests under NSS wrapper"
	POST_BUILD COMMAND
		/usr/bin/env
			LD_PRELOAD=libnss_wrapper.so
			NSS_WRAPPER_PASSWD=${WRAPPER_PASSWD}
			NSS_WRAPPER_GROUP=${WRAPPER_GROUP}
			NSS_WRAPPER_HOSTS=${WRAPPER_HOSTS}
			NSS_WRAPPER_DEBUGLEVEL=3
			$<TARGET_FILE:test_main>
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	BYPRODUCTS
		${CMAKE_CURRENT_BINARY_DIR}/data/role.test.add.role
		${CMAKE_CURRENT_BINARY_DIR}/data/role.test.del.role
		${CMAKE_CURRENT_BINARY_DIR}/data/role.test.drop.role
		${CMAKE_CURRENT_BINARY_DIR}/data/role.test.int.new.role
		${CMAKE_CURRENT_BINARY_DIR}/data/role.test.set.role
		${CMAKE_CURRENT_BINARY_DIR}/data/role.test.string.new.role
	VERBATIM)
