cmake_minimum_required(VERSION 2.6)
project(wikipp)
macro (set_rpath target)
	if(UNIX)
		set_target_properties(${target} PROPERTIES
			BUILD_WITH_INSTALL_RPATH TRUE
			INSTALL_RPATH "$ORIGIN;$ORIGIN/../lib;$ORIGIN/.."
			)
	endif()
endmacro()

include(CPack)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
	      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

option(USE_STATIC_VIEW "Compile view statically into wikipp" OFF)

if(WIN32 OR CYGWIN)
	add_definitions(-DDLL_EXPORT)
endif()


find_library(CPPCMS cppcms)
find_library(BOOSTER booster)
find_library(CPPDB cppdb)
find_library(DISCOUNT markdown)

find_path(CPPCMS_INC cppcms/application.h)
find_path(BOOSTER_INC booster/shared_ptr.h)
find_path(CPPDB_INC cppdb/frontend.h)
find_path(DISCOUNT_INC mkdio.h)

if(NOT DISCOUNT_INC OR NOT DISCOUNT)
	message(FATAL 	"-- Discount markdown library was not found, please download it from \n"
			"   http://www.pell.portland.or.us/~orc/Code/discount/ and install it\n"
			"   By extracting it and running ./configure.sh && make CFLAGS=-O2\n")
endif()


include_directories(${CPPCMS_INC})
include_directories(${BOOSTER_INC})
include_directories(${CPPDB_INC})
include_directories(${DISCOUNT_INC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

find_program(TMPLCC cppcms_tmpl_cc)
find_program(XGETTEXT xgettext)
find_program(MSGFMT msgfmt)
find_program(MSGMERGE msgmerge)

if(WIN32)
	add_definitions(-DDLL_EXPORT)
endif()


set(TEMPLATES 
	${CMAKE_CURRENT_SOURCE_DIR}/templates/main.tmpl
	${CMAKE_CURRENT_SOURCE_DIR}/templates/page.tmpl
	${CMAKE_CURRENT_SOURCE_DIR}/templates/hist.tmpl
	${CMAKE_CURRENT_SOURCE_DIR}/templates/admin.tmpl
	${CMAKE_CURRENT_SOURCE_DIR}/templates/toc.tmpl
)

set(SRC 
	src/index.cpp
	src/main.cpp
	src/master.cpp
	src/options.cpp
	src/page.cpp
	src/users.cpp
	src/wiki.cpp
	src/markdown.cpp
)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/view.cpp
	COMMAND ${TMPLCC}
		-d wikipp
		-o ${CMAKE_CURRENT_BINARY_DIR}/view.cpp 
		${TEMPLATES}
	DEPENDS ${TEMPLATES})


if(USE_STATIC_VIEW)
	add_executable(wikipp ${SRC} ${CMAKE_CURRENT_BINARY_DIR}/view.cpp)
else()
	add_executable(wikipp ${SRC})
	add_library(view SHARED ${CMAKE_CURRENT_BINARY_DIR}/view.cpp)
	set_rpath(view)
	target_link_libraries(view ${BOOSTER} ${CPPCMS})
endif()
set_rpath(wikipp)


target_link_libraries(wikipp ${BOOSTER} ${CPPCMS} ${CPPDB} ${DISCOUNT})

#  Localization


set(LOCALES he ru pl)

set(MO_FILES)
set(UPDATE_PO_LIST)
set(POT_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/po/wikipp.pot")

add_custom_command(
	OUTPUT ${POT_TEMPLATE}
	COMMAND 
		${XGETTEXT} 
		--keyword=translate:1,1t
		--keyword=translate:1,2,3t
		--keyword=_
		--keyword=N_
		${SRC}
		${CMAKE_CURRENT_BINARY_DIR}/view.cpp
		--output=${POT_TEMPLATE}
	DEPENDS ${SRC} ${CMAKE_CURRENT_BINARY_DIR}/view.cpp
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)

add_custom_target(update-po)

foreach(LOCALE ${LOCALES})
	set(MODIR "${CMAKE_CURRENT_BINARY_DIR}/locale/${LOCALE}/LC_MESSAGES")
	file(MAKE_DIRECTORY "${MODIR}")
	set(MOFILE "${MODIR}/wikipp.mo")
	set(POFILE "${CMAKE_CURRENT_SOURCE_DIR}/po/${LOCALE}.po")
	
	add_custom_command(
		OUTPUT ${MOFILE}
		COMMAND ${MSGFMT} ${POFILE} -o ${MOFILE}
		DEPENDS ${POFILE})
	
	add_custom_target(update-po-${LOCALE}
		COMMAND ${MSGMERGE} -U ${POFILE} ${CMAKE_CURRENT_SOURCE_DIR}/po/wikipp.pot 
		DEPENDS ${POT_TEMPLATE}
		)
	add_dependencies(update-po update-po-${LOCALE})

	set(MO_FILES ${MO_FILES} ${MOFILE})
	set(UPDATE_PO_LIST ${UPDATE_PO_LIST} update-po-${LOCALE})
endforeach()

add_custom_target(create-po ALL DEPENDS ${MO_FILES})

add_executable(wikipp_migrate src/migrate.cpp)
set_rpath(wikipp_migrate)
target_link_libraries(wikipp_migrate ${CPPDB})


install(TARGETS wikipp wikipp_migrate view
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib/wikipp
	ARCHIVE DESTINATION lib/wikipp)

foreach(STYLE style style-ltr style-rtl) 
	install(FILES media/${STYLE}.css DESTINATION share/wikipp/media)
endforeach()
foreach(LOCALE ${LOCALES})
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/locale/${LOCALE}/LC_MESSAGES/wikipp.mo 
		DESTINATION share/locale/${LOCALE}/LC_MESSAGES/)
endforeach()
install(FILES sample_config.js DESTINATION share/wikipp)

