set(GravSimul_SOURCE_FILES
	math/math.cpp
	phys/body.cpp
	phys/geometry.cpp
	phys/phys.cpp
	render/camera.cpp
	render/shader.cpp
	render/shaderSetup.cpp)

set(GravSimul_HEADER_FILES
	math/math.h
	phys/body.h
	phys/geometry.h
	phys/phys.h
	render/camera.h
	render/shader.h
	render/shaderSetup.h)

add_library(GravSimul STATIC ${GravSimul_SOURCE_FILES} ${GravSimul_HEADER_FILES})
target_include_directories(GravSimul PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

