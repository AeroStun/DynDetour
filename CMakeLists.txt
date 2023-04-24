cmake_minimum_required (VERSION 3.16)

project (dyndetour)

include (FetchContent)
FetchContent_Declare (
        dlhook
        GIT_REPOSITORY https://github.com/AeroStun/dlhook.git
        GIT_TAG        master
)
FetchContent_MakeAvailable (dlhook)

add_library (dyndetour INTERFACE)
target_include_directories (dyndetour INTERFACE include)
target_link_libraries (dyndetour INTERFACE dlhook)
target_sources (dyndetour INTERFACE include/detour.hxx)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include (CTest)
    if (BUILD_TESTING)
        add_subdirectory (test)
    endif ()
endif ()
