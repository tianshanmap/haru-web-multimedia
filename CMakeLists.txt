# set minimum cmake version
cmake_minimum_required(VERSION 4.2 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET 26.0)

# project name and language
project(web-multimedia LANGUAGES CXX)

# 1. Find the OpenCV package
find_package(OpenCV REQUIRED)

#yaml-cpp dependencies
include(FetchContent)
FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG master # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(yaml-cpp)
#simple-logger dependencies
include(FetchContent)
FetchContent_Declare(
        simplelogger
        GIT_REPOSITORY https://github.com/tianshanmap/haru-simplelogger.git
        GIT_TAG main # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(simplelogger)

#file-utils dependencies
include(FetchContent)
FetchContent_Declare(
        file_utils
        GIT_REPOSITORY https://github.com/tianshanmap/haru-file-utils.git
        GIT_TAG main # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(file_utils)

#file-utils dependencies
include(FetchContent)
FetchContent_Declare(
        haru_ffmpeg
        GIT_REPOSITORY https://github.com/tianshanmap/haru-ffmpeg.git
        GIT_TAG main # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(haru_ffmpeg)

# main code
add_executable(web-multimedia
        main.cpp
        modules/opencv_utils.cpp
        modules/opencv_utils.h
        modules/haru_random.h
        modules/photo_handler.cpp
        modules/photo_handler.h
        modules/haru_httpserver.cpp
        modules/haru_yaml.h
        modules/haru_httpserver.h
        include/json.hpp
        modules/web_models.h
        modules/haru_http_handlers.cpp
        modules/haru_http_handlers.h
        modules/haru_service.cpp
        modules/haru_service.h
)

# 3. Link OpenCV libraries to your executable
target_link_libraries(web-multimedia ${OpenCV_LIBS} yaml-cpp::yaml-cpp simplelogger file_utils haru_ffmpeg)
target_include_directories(web-multimedia PRIVATE ${OpenCV_INCLUDE_DIRS})
