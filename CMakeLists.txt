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

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)

if(WIN32)
    set(PostgreSQL_ROOT "C:/Users/Administrator/Desktop/postgresql-18.4-1-windows-x64-binaries/pgsql" CACHE PATH "Path to PostgreSQL installation")
endif()
find_package(PostgreSQL REQUIRED)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/app/Application.cpp
    src/ui/ConnectionPanel.cpp
    src/ui/DocumentTabs.cpp
    src/ui/ResultsTable.cpp
    src/db/postgres/PostgresConnection.cpp
        src/ui/Controls.h
        src/ui/Controls.cpp
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
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)
