cmake_minimum_required(VERSION 3.16)

project(GravitySim LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_definitions(NOMINMAX)
endif()

option(GRAVITY_BUILD "Build the GravitySimul program" ON)

if (GRAVITY_BUILD)
    # GLFW configuration
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

    add_subdirectory(extern/glad)
    add_subdirectory(extern/glfw)
    add_subdirectory(extern/imgui)

    add_subdirectory(src)

    add_executable(GravitySimApp src/main.cpp)
    target_link_libraries(GravitySimApp PRIVATE GravSimul glfw glad imgui)

    if (WIN32)
        target_link_libraries(GravitySimApp PRIVATE opengl32)
    endif()

    if (MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GravitySimApp)
    endif()
endif()
