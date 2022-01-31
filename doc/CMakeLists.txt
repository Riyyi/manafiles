# Compress the man page and install to proper path

find_program(GZIP_TOOL
	NAMES gzip
	PATHS /bin
	      /usr/bin
	      /usr/local/bin)

if(NOT GZIP_TOOL)
    message(FATAL_ERROR "Man page compression executable 'gzip' not found.")
endif()

add_custom_target(doc ALL
	COMMAND ${GZIP_TOOL} -c ${CMAKE_SOURCE_DIR}/doc/manafiles.1 > ${CMAKE_BINARY_DIR}/manafiles.1.gz
	DEPENDS ${CMAKE_SOURCE_DIR}/doc/manafiles.1)

install(
	FILES ${CMAKE_BINARY_DIR}/manafiles.1.gz
	DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
	CONFIGURATIONS Release)
