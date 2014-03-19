
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(fw_name "location-appman")

PROJECT(${fw_name})

SET(CMAKE_INSTALL_PREFIX /usr)
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(DBDIR ${TZ_SYS_DB})

SET(INC_DIR include)
INCLUDE_DIRECTORIES(${INC_DIR})

INCLUDE(FindPkgConfig)
pkg_check_modules(${fw_name} REQUIRED dlog sqlite3 glib-2.0 libxml-2.0 capi-appfw-app-manager)
FOREACH(flag ${${fw_name}_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Wall -Werror")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

IF("${ARCH}" STREQUAL "arm")
    ADD_DEFINITIONS("-DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DTIZEN_DEBUG")
ADD_DEFINITIONS("-DLOCATION_APPMAN_DBDIR=\"${DBDIR}\"")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl,--rpath=${LIB_INSTALL_DIR}")

aux_source_directory(src SOURCES)
ADD_LIBRARY(${fw_name} SHARED ${SOURCES})

SET_TARGET_PROPERTIES(${fw_name}
     PROPERTIES
     SOVERSION 0.1.0
     CLEAN_DIRECT_OUTPUT 1
)

TARGET_LINK_LIBRARIES(${fw_name} ${${fw_name}_LDFLAGS} "-lgcrypt")

INSTALL(TARGETS ${fw_name} DESTINATION ${LIB_INSTALL_DIR})
INSTALL(
        DIRECTORY ${INC_DIR}/ DESTINATION ${INCLUDE_INSTALL_DIR}/location-appman
        FILES_MATCHING
        PATTERN "${INC_DIR}/location-appman.h"
        PATTERN "${INC_DIR}/location-appman-crypt.h"
        )

EXEC_PROGRAM(sqlite3 ARGS "${CMAKE_BINARY_DIR}/.location-appman.db < ${CMAKE_SOURCE_DIR}/location-appman.sql" RETURN_VALUE RET)
IF (${RET})
	MESSAGE(SEND_ERROR "sqlite3 failed")
ENDIF(${RET})

INSTALL(FILES ${CMAKE_BINARY_DIR}/.location-appman.db DESTINATION ${DBDIR})
INSTALL(FILES ${CMAKE_BINARY_DIR}/.location-appman.db-journal DESTINATION ${DBDIR})

SET(PC_NAME ${fw_name})
SET(PC_REQUIRED ${pc_dependents})
SET(PC_LDFLAGS -l$"location-appman")
SET(PC_CFLAGS -I\${includedir}/location)

CONFIGURE_FILE(
    ${fw_name}.pc.in
    ${CMAKE_CURRENT_SOURCE_DIR}/${fw_name}.pc
    @ONLY
)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${fw_name}.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)

#create db
#INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/location-appman.sql DESTINATION ${ETCDIR})

ADD_SUBDIRECTORY(test)

IF(UNIX)

ADD_CUSTOM_TARGET (distclean @echo cleaning for source distribution)
ADD_CUSTOM_COMMAND(
        DEPENDS clean
        COMMENT "distribution clean"
        COMMAND find
        ARGS    .
        -not -name config.cmake -and \(
        -name tester.c -or
        -name Testing -or
        -name CMakeFiles -or
        -name cmake.depends -or
        -name cmake.check_depends -or
        -name CMakeCache.txt -or
        -name cmake.check_cache -or
        -name *.cmake -or
        -name Makefile -or
        -name core -or
        -name core.* -or
        -name gmon.out -or
        -name install_manifest.txt -or
        -name *.pc -or
        -name *~ \)
        | grep -v TC | xargs rm -rf
        TARGET  distclean
        VERBATIM
)

ENDIF(UNIX)

