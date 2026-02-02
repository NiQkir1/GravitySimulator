cmake_minimum_required(VERSION 3.8)

project(GravitySim LANGUAGES CXX)

add_subdirectory(src)

option(GRAVITY_BUILD "Build the GravitySimul program" ON)

if (GRAVITY_BUILD)
    add_subdirectory(extern/glad/include)
    add_subdirectory(extern/glfw)
    add_subdirectory(extern/imgui)

    add_executable(GravitySimApp src/main.cpp)
    target_link_libraries(GravitySimApp PRIVATE GravSimul glfw glad imgui)

    if (MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT GravitySimApp)
    endif()
endif()
