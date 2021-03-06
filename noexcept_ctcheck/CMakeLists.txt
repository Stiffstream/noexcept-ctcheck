cmake_minimum_required(VERSION 3.5)

SET(NOEXCEPT_CTCHECK noexcept_ctcheck)
SET(NOEXCEPT_CTCHECK_VERSION "1.0.0")

if(NOT NOEXCEPT_CTCHECK_MASTER_PROJECT)
	set(NOEXCEPT_CTCHECK_MASTER_PROJECT OFF)
	if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
		set(NOEXCEPT_CTCHECK_MASTER_PROJECT ON)
	endif()
endif()

if(NOT NOEXCEPT_CTCHECK_INSTALL)
	set(NOEXCEPT_CTCHECK_INSTALL ${NOEXCEPT_CTCHECK_MASTER_PROJECT})
endif()

project(${NOEXCEPT_CTCHECK}
	VERSION ${NOEXCEPT_CTCHECK_VERSION}
	LANGUAGES CXX)

add_library(${NOEXCEPT_CTCHECK} INTERFACE)
add_library(noexcept_ctcheck::core ALIAS ${NOEXCEPT_CTCHECK})

IF (${CMAKE_VERSION} VERSION_LESS "3.8.0")
	set(CMAKE_CXX_STANDARD 11)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS OFF)
ELSE ()
	target_compile_features(${NOEXCEPT_CTCHECK} INTERFACE cxx_std_11)
ENDIF ()

get_filename_component(CURRENT_FILE_DIR ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)
get_filename_component(CURRENT_FILE_DIR ${CURRENT_FILE_DIR} DIRECTORY)
SET(NOEXCEPT_CTCHECK_INCLUDE_PATH ${CURRENT_FILE_DIR})
UNSET(CURRENT_FILE_DIR)

# Propagate include directories.
TARGET_INCLUDE_DIRECTORIES(${NOEXCEPT_CTCHECK}
	INTERFACE
		$<BUILD_INTERFACE:${NOEXCEPT_CTCHECK_INCLUDE_PATH}>
		$<INSTALL_INTERFACE:include>
	)

SET(NOEXCEPT_CTCHECK_HEADERS_ALL
	pub.hpp
	)

IF (NOEXCEPT_CTCHECK_INSTALL)
	include(GNUInstallDirs)
	include(CMakePackageConfigHelpers)

	SET(NOEXCEPT_CTCHECK_CMAKE_DIR
		${CMAKE_INSTALL_LIBDIR}/cmake/noexcept_ctcheck CACHE STRING
		"Installation directory for cmake files, relative to ${CMAKE_INSTALL_PREFIX}.")
	SET(NOEXCEPT_CTCHECK_VERSION_CONFIG
		${PROJECT_BINARY_DIR}/noexcept_ctcheck-config-version.cmake)
	SET(NOEXCEPT_CTCHECK_PROJECT_CONFIG
		${PROJECT_BINARY_DIR}/noexcept_ctcheck-config.cmake)

	SET(NOEXCEPT_CTCHECK_INC_DIR
		${CMAKE_INSTALL_INCLUDEDIR}/noexcept_ctcheck/ CACHE STRING
		"Installation directory for include files, relative to ${CMAKE_INSTALL_PREFIX}.")

	# Generate the version, config and target files into the build directory.
	write_basic_package_version_file(
		${NOEXCEPT_CTCHECK_VERSION_CONFIG}
		VERSION ${NOEXCEPT_CTCHECK_VERSION}
		COMPATIBILITY ExactVersion )

	configure_package_config_file(
		"cmake/noexcept_ctcheck-config.cmake.in"
		${NOEXCEPT_CTCHECK_PROJECT_CONFIG}
		INSTALL_DESTINATION ${NOEXCEPT_CTCHECK_CMAKE_DIR}
		PATH_VARS NOEXCEPT_CTCHECK_VERSION)

	# Install version, config and target files.
	install(
		FILES ${NOEXCEPT_CTCHECK_PROJECT_CONFIG}
				${NOEXCEPT_CTCHECK_VERSION_CONFIG}
		DESTINATION ${NOEXCEPT_CTCHECK_CMAKE_DIR})

	SET(NOEXCEPT_CTCHECK_INSTALL_TARGETS ${NOEXCEPT_CTCHECK})

	install(
		TARGETS ${NOEXCEPT_CTCHECK_INSTALL_TARGETS}
		EXPORT NOEXCEPT_CTCHECK_ALL_TARGETS
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin
	)

	install(
		EXPORT NOEXCEPT_CTCHECK_ALL_TARGETS
		NAMESPACE noexcept_ctcheck::
		FILE noexcept_ctcheck-targets.cmake
		DESTINATION ${NOEXCEPT_CTCHECK_CMAKE_DIR} )

	FOREACH (HEADER_FILE ${NOEXCEPT_CTCHECK_HEADERS_ALL} )
		get_filename_component( DIR ${HEADER_FILE} PATH )
		install(
			FILES ${HEADER_FILE}
			DESTINATION ${NOEXCEPT_CTCHECK_INC_DIR}/${DIR} )
	ENDFOREACH ()

ENDIF ()

