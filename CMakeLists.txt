cmake_minimum_required(VERSION 3.14) # min cmake version

project(ShowMetadata
    VERSION 1.0
    DESCRIPTION ""
    LANGUAGES CXX)

set(VERSION "1.0.1")
configure_file(${PROJECT_SOURCE_DIR}/src/ShowMetadataConfig.h.in ${PROJECT_SOURCE_DIR}/src/ShowMetadataConfig.h)

# ffmpeg is added with 'find_path' and 'find_library'

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/dict.h)
find_library(AVUTIL_LIBRARY avutil)

find_package(Boost REQUIRED COMPONENTS program_options) # 1.74.0

if(Boost_FOUND)
    add_executable(ShowMetadata
        src/ShowMetadata.cpp
    )
    target_compile_options(ShowMetadata PRIVATE -Wall -Wextra)
    target_include_directories(ShowMetadata
        PRIVATE ${AVFORMAT_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR}
    )
    target_link_libraries(ShowMetadata PRIVATE ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY} Boost::program_options)
endif()

