cmake_minimum_required(VERSION 3.20)
project(untitled CXX)

set(CMAKE_CXX_STANDARD 17)

# Fetch SFML via direct ZIP download
include(FetchContent)
FetchContent_Declare(
        SFML
        URL https://www.sfml-dev.org/files/SFML-2.6.1-sources.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(SFML)

# Add executable
add_executable(untitled robotcollison.cpp)

# Link SFML
target_link_libraries(untitled PRIVATE sfml-graphics sfml-window sfml-system)

# AUTOMATICALLY COPY SFML DLLs NEXT TO YOUR EXE
if (WIN32)
    add_custom_command(TARGET untitled POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:untitled>
            $<TARGET_FILE_DIR:untitled>
            COMMAND_EXPAND_LISTS
    )
endif()