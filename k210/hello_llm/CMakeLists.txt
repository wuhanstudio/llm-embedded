set(BUILDING_SDK "yes" CACHE INTERNAL "")

set(SDK ../kendryte-freertos-sdk)

## Required Variable: (if you do not use Kendryte IDE)
# -DTOOLCHAIN=/path/to/rsicv/toolchain/bin
# -DSDK=/path/to/SDK (the folder of this example file)

cmake_minimum_required(VERSION 3.0)

## Include the SDK library
include("${SDK}/cmake/common.cmake")

set(PROJ hello_llm)
project(${PROJ})

INCLUDE_DIRECTORIES(${SDK}/lib/arch/include ${SDK}/lib/utils/include)

# config self use headers
header_directories(${SDK}/lib)

# build library first
add_subdirectory(${SDK}/lib sdk)
add_subdirectory(${SDK}/third_party third_party)

## Use SDK library API to add source files
add_source_files(llm.c main.c)

# other cmake instructions

## Use SDK builder to build this project
include("${SDK}/cmake/executable.cmake")
