project(imgui CXX)

set(IMGUI_CORE_SOURCES
    imgui.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    imgui_tables.cpp
    imgui_demo.cpp
    imgui_impl_glfw.cpp
    imgui_impl_opengl3.cpp
)

set(IMGUI_CORE_HEADERS
    imconfig.h
    imgui.h
    imgui_internal.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
    imgui_impl_glfw.h
    imgui_impl_opengl3.h
    imgui_impl_opengl3_loader.h
)

add_library(imgui STATIC ${IMGUI_CORE_SOURCES} ${IMGUI_CORE_HEADERS})

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(imgui PUBLIC glfw glad)

if (WIN32)
    target_link_libraries(imgui PUBLIC opengl32)
endif()
