cmake_minimum_required(VERSION 3.27.7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# paths
set(src_path $ENV{src_path})
set(bin_path $ENV{bin_path})

# project, exe name
set(project_name $ENV{exe_name})
project(${project_name})

# output dir for binaries , exes
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${bin_path})

# headers paths
include_directories(${src_path})

add_executable(
${project_name}
${src_path}/main.cpp
${src_path}/Generator/Generator.cpp
${src_path}/Analyzer/Analyzer.cpp
${src_path}/Engine.cpp
)


# Build type (RELEASE , DEBUG)
set(CMAKE_BUILD_TYPE DEBUG)