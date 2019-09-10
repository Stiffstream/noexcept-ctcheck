cmake_minimum_required(VERSION 3.5)

set(PRJ noexcept_ctcheck_example)

project(${PRJ} CXX)

add_executable(${PRJ} main.cpp)
target_link_libraries(${PRJ} PRIVATE noexcept_ctcheck::core)

install(
	TARGETS ${PRJ}
	RUNTIME DESTINATION bin
)
