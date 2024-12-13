cmake_minimum_required(VERSION 3.27)
project(fmisql
	VERSION 0.1.0
	DESCRIPTION "FMISql"
	HOMEPAGE_URL ""
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSION FALSE)

add_executable(fmisql
	src/main.cpp
)

if (MSVC)
	# compliant by default?
else()
	target_compile_options(fmisql PRIVATE -pedantic-errors)
endif()
