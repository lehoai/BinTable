cmake_minimum_required(VERSION 3.20)
project(amgui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force O2 in Release builds (MSVC defaults to /O2 already, GCC/Clang default to -O3)
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG master
)
FetchContent_MakeAvailable(imgui)

#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
#
#FetchContent_Declare(
#    glfw
#    GIT_REPOSITORY https://github.com/glfw/glfw.git
#    GIT_TAG 3.4
#)
#FetchContent_MakeAvailable(glfw)

set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.10
)
FetchContent_MakeAvailable(SDL3)

find_package(OpenGL REQUIRED)

# Prefer a system-installed FreeType (fast, e.g. on Linux).
# Fall back to building it from source via FetchContent (e.g. on Windows
# where FreeType is usually not installed).
find_package(Freetype QUIET)
if(NOT Freetype_FOUND)
    message(STATUS "System FreeType not found; fetching and building from source")
    FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://github.com/freetype/freetype.git
        GIT_TAG VER-2-13-3
    )
    FetchContent_MakeAvailable(freetype)
    # FreeType's in-tree build only exposes the lowercase `freetype` target;
    # the namespaced `Freetype::Freetype` name exists only when installed.
    # Provide the alias so downstream link lines stay unchanged.
    if(NOT TARGET Freetype::Freetype)
        add_library(Freetype::Freetype ALIAS freetype)
    endif()
endif()

if(WIN32)
    set(PostgreSQL_ROOT "C:/Users/Administrator/Desktop/postgresql-18.4-1-windows-x64-binaries/pgsql" CACHE PATH "Path to PostgreSQL installation")
endif()
find_package(PostgreSQL REQUIRED)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
    ${imgui_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${imgui_SOURCE_DIR}/misc/freetype
)

target_compile_definitions(imgui PUBLIC IMGUI_ENABLE_FREETYPE)

target_link_libraries(imgui PUBLIC SDL3::SDL3 OpenGL::GL Freetype::Freetype)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/app/Application.cpp
        src/ui/NewConnectionPopup.cpp
    src/ui/DocumentTabs.cpp
    src/ui/ResultsTable.cpp
    src/db/postgres/PostgresConnection.cpp
        src/ui/Controls.h
        src/ui/Controls.cpp
        src/db/ConnectionFactory.h
        src/db/ConnectionFactory.cpp
        src/db/ConnectionManager.cpp
        src/db/ConnectionManager.h
        src/ui/SideBar/SideBar.cpp
        src/ui/SideBar/SideBar.h
        src/services/ConnectionService.cpp
        src/services/ConnectionService.h
        src/ui/dark_style.h
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PRIVATE imgui PostgreSQL::PostgreSQL)

if(WIN32)
    get_filename_component(POSTGRESQL_BIN_DIR "${PostgreSQL_LIBRARY_DIRS}/../bin" ABSOLUTE)
    set(POSTGRESQL_RUNTIME_DLLS libpq.dll libcrypto-3-x64.dll libssl-3-x64.dll libintl-9.dll libiconv-2.dll)
    foreach(dll ${POSTGRESQL_RUNTIME_DLLS})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${POSTGRESQL_BIN_DIR}/${dll}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        )
    endforeach()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)
