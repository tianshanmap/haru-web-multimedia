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
        modules/HaruRouter.cpp
        modules/HaruRouter.h
)

# 3. Link OpenCV libraries to your executable
target_link_libraries(web-multimedia ${OpenCV_LIBS} yaml-cpp::yaml-cpp simplelogger file_utils haru_ffmpeg)
target_include_directories(web-multimedia PRIVATE ${OpenCV_INCLUDE_DIRS})
# Assuming your main executable target is named "web-multimedia"
if(APPLE)
    # macOS Setup
    message(STATUS "On Mac...")
    set_target_properties(web-multimedia PROPERTIES
            MACOSX_RPATH TRUE
            INSTALL_RPATH "@loader_path/../lib;/opt/homebrew/lib"
    )
else()
    # Linux (Ubuntu) Setup
    message(STATUS "On Linux...")
    set_target_properties(web-multimedia PROPERTIES
            # Note the escaped \\$ORIGIN so CMake treats it as a literal string for the linker
            INSTALL_RPATH "$ORIGIN/../lib:/opt/homebrew/lib"
    )
endif()

install(TARGETS web-multimedia
        RUNTIME DESTINATION bin  # Installs the app to the 'bin' folder
)
#install(CODE [[
#    file(GET_RUNTIME_DEPENDENCIES
#        EXECUTABLES $<TARGET_FILE:web-multimedia>
#        RESOLVED_DEPENDENCIES_VAR resolved_deps
#        UNRESOLVED_DEPENDENCIES_VAR unresolved_deps
#        # Exclude standard system and kernel libraries so we only grab OpenCV/third-party libs
#        PRE_EXCLUDE_REGEXES "^/lib" "^/usr/lib" "^/System" "^/usr/lib/system"
#    )
#    foreach(file ${resolved_deps})
#        # Filter to ensure we are only copying OpenCV libraries
#        if(file MATCHES "opencv")
#            message(STATUS "Bundling dependency: ${file}")
#            file(INSTALL ${file} DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
#        endif()
#    endforeach()
#]])
install(TARGETS haru_ffmpeg
        EXPORT web-multimedia
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)
install(TARGETS simplelogger
        EXPORT web-multimedia
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)
install(TARGETS file_utils
        EXPORT web-multimedia
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)
