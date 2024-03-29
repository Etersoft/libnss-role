cmake_minimum_required(VERSION 3.10)
project(libnss-role
	LANGUAGES C
	VERSION 0.5.5
	DESCRIPTION "NSS library to work with roles")
set(C_STANDARD 90)
set(CMAKE_C_STANDARD 90)
set(C_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

add_compile_options(-pedantic -g3)

option(ENABLE_TESTS "Enable unit tests for libnss-role" ON)
option(NSS_LIBDIR "Destination directory for libnss_role" "/lib64")
option(ROLE_LIBDIR "Destination directory for librole" "/usr/lib64")
option(MANDIR "Manpage install directory" "/usr/share/man/man8")

if(ENABLE_TESTS)
	find_package(CMocka CONFIG REQUIRED)
	include(AddCMockaTest)
	include(AddMockedTest)
	enable_testing()
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()

set(NSS_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/common.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/lock_file.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/pam_check.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/parser.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/paths.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/fileop.c)

set(MANPAGES
	${CMAKE_CURRENT_SOURCE_DIR}/doc/rolelst.8
	${CMAKE_CURRENT_SOURCE_DIR}/doc/roleadd.8
	${CMAKE_CURRENT_SOURCE_DIR}/doc/roledel.8)

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/include/role/fileop.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/role/fileop_rw.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/role/glob.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/role/lock_file.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/role/pam_check.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/role/paths.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/role/parser.h)

set(LIBROLE_VERSION 1.0.0)
set(LIBROLE_SOVERSION 1)
set(LIBNSSROLE_VERSION 2.0.0)
set(LIBNSSROLE_SOVERSION 2)

configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/role/version.h)

include_directories(BEFORE
	${CMAKE_CURRENT_BINARY_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(role SHARED
	${CMAKE_CURRENT_SOURCE_DIR}/src/fileop_rw.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/graph.c
	${NSS_SOURCES})

target_link_libraries(role pam pam_misc)

set_target_properties(role PROPERTIES
	VERSION ${LIBROLE_VERSION}
	SOVERSION ${LIBROLE_SOVERSION})

add_library(nss_role SHARED
	${CMAKE_CURRENT_SOURCE_DIR}/src/nss_role.c
	${NSS_SOURCES})

target_link_libraries(nss_role pam pam_misc)

set_target_properties(nss_role PROPERTIES
	VERSION ${LIBNSSROLE_VERSION}
	SOVERSION ${LIBNSSROLE_SOVERSION})

add_executable(roleadd
	${CMAKE_CURRENT_SOURCE_DIR}/src/roleadd.c)

target_link_libraries(roleadd role pam pam_misc)

add_executable(roledel
	${CMAKE_CURRENT_SOURCE_DIR}/src/roledel.c)

target_link_libraries(roledel role pam pam_misc)

add_executable(rolelst
	${CMAKE_CURRENT_SOURCE_DIR}/src/rolelst.c)

target_link_libraries(rolelst role pam pam_misc)

add_executable(checkver ${CMAKE_CURRENT_SOURCE_DIR}/src/checkver.c)

install(TARGETS rolelst
	RUNTIME DESTINATION bin)
install(TARGETS roledel roleadd
	RUNTIME DESTINATION sbin)
install(TARGETS role
	LIBRARY DESTINATION ${ROLE_LIBDIR})
install(TARGETS nss_role
	LIBRARY DESTINATION ${NSS_LIBDIR}
	NAMELINK_SKIP)

install(FILES ${HEADERS}
	DESTINATION include/role)
install(FILES ${MANPAGES}
	DESTINATION ${MANDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dist/role
	DESTINATION /etc)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dist/role.pamd
	DESTINATION /etc/pam.d
	RENAME roleadd)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dist/role.pamd
	DESTINATION /etc/pam.d
	RENAME roledel)
