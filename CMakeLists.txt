cmake_minimum_required(VERSION 3.10)

project(Grend VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(PHYSICS_BULLET "Use the bullet physics library" ON)
option(GREND_ERROR_CHECK "Enable graphics library error checks" ON)
set(GLSL_VERSION 330)

add_subdirectory(libs/nanovg)
find_package(SDL2 REQUIRED COMPONENTS SDL2::SDL2)
find_package(Bullet REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2     REQUIRED IMPORTED_TARGET sdl2)
pkg_check_modules(SDL2_ttf REQUIRED IMPORTED_TARGET SDL2_ttf)
pkg_check_modules(Bullet   REQUIRED IMPORTED_TARGET bullet)
pkg_check_modules(Glew     REQUIRED IMPORTED_TARGET glew)
pkg_check_modules(GLM      REQUIRED IMPORTED_TARGET glm)

set(INCLUDES
	include/grend
	libs
	libs/tinygltf
	libs/nanovg/src
	libs/imgui
	libs/json/single_include
	libs/stb)

include_directories(include)
include_directories(libs)
include_directories(libs/tinygltf)
include_directories(libs/nanovg/src)
include_directories(libs/imgui)
include_directories(libs/json/single_include)
include_directories(libs/stb)
include_directories(PkgConfig::GLM)

add_compile_definitions(
	# GLSL version is also used to determine OpenGL API version
	# GLSL_VERSION=100 # OpenGL ES 2.0
	# GLSL_VERSION=300 # OpenGL ES 3.0
	GLSL_VERSION=330 # OpenGL core 3.30
	# GLSL_VERSION=430 # OpenGL core 4.30
	IMGUI_IMPL_OPENGL_LOADER_GLEW
	GR_PREFIX="${PROJECT_SOURCE_DIR}/test-install/share/grend/"
)
if (PHYSICS_BULLET)
	add_compile_definitions(PHYSICS_BULLET)
	set(PHYSICS_IMPLEMENTATION src/bulletPhysics.cpp)
else()
	add_compile_definitions(PHYSICS_IMP)
	set(PHYSICS_IMPLEMENTATION src/impPhysics.cpp)
endif()

configure_file(config.h.in config.h)

#link_directories(${CMAKE_SOURCE_DIR}/libs/nanovg/build)
add_library(Grend 
	${PHYSICS_IMPLEMENTATION}
	src/animation.cpp
	src/controllers.cpp
	src/gameObject.cpp
	src/octree.cpp
	src/quadtree.cpp
	src/utility.cpp
	src/audioMixer.cpp
	src/camera.cpp
	src/editorInput.cpp
	src/editorMap.cpp
	src/editorMapWindow.cpp
	src/editorMenubar.cpp
	src/editorMetricsWindow.cpp
	src/editorObjectEditorWindow.cpp
	src/editorObjectSelectWindow.cpp
	src/fileDialog.cpp
	src/geometryGeneration.cpp
	src/glManager.cpp
	src/glManagerBuffer.cpp
	src/glManagerShader.cpp
	src/glManagerTexture.cpp
	src/mainLogic.cpp
	src/modalSDLInput.cpp
	src/model.cpp
	src/renderer.cpp
	src/rendererProbes.cpp
	src/sdlContext.cpp
	src/spatialAudioChannel.cpp
	src/text.cpp
	src/textureAtlas.cpp
	src/timers.cpp
	src/vecGUI.cpp
	src/gameMainDevWindow.cpp
	src/jobQueue.cpp
	src/bufferAllocator.cpp
	src/gameMain.cpp
	src/renderFramebuffer.cpp
	src/plane.cpp
	src/gltfModel.cpp
	src/objModel.cpp
	src/editor.cpp
	src/skybox.cpp
	src/ecsEntityManager.cpp
	src/ecsCollision.cpp
	src/ecsRigidBody.cpp
	src/gameView.cpp
	src/ecsSerializer.cpp
	src/editorEditorSelectWindow.cpp
	src/renderQueue.cpp
	src/playerView.cpp
	src/tinygltf.cpp

	libs/imgui/imgui.cpp
	libs/imgui/imgui_demo.cpp
	libs/imgui/imgui_draw.cpp
	libs/imgui/imgui_widgets.cpp
	libs/imgui/examples/imgui_impl_opengl3.cpp
	libs/imgui/examples/imgui_impl_sdl.cpp

	libs/stb/stbi.cpp
	libs/stb/stb_vorbis.c
)

target_include_directories(Grend PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(Grend
	PkgConfig::SDL2
	PkgConfig::SDL2_ttf
	PkgConfig::Bullet
	PkgConfig::Glew
	nanovg
)

include_directories(demos/landscape-test/)
add_executable(landscape-test
	demos/landscape-test/boxSpawner.cpp
	demos/landscape-test/enemy.cpp
	demos/landscape-test/healthbar.cpp
	demos/landscape-test/inputHandler.cpp
	demos/landscape-test/landscapeGenerator.cpp
	demos/landscape-test/main.cpp
	demos/landscape-test/player.cpp
	demos/landscape-test/projectile.cpp
)

target_include_directories(landscape-test PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(landscape-test Grend)

include_directories(demos/map-viewer/)
add_executable(map-viewer demos/map-viewer/main.cpp)
target_link_libraries(map-viewer Grend)

# TODO: need to have library includes under grend folder
install(TARGETS Grend)
install(TARGETS landscape-test)
install(TARGETS map-viewer)
install(DIRECTORY ${INCLUDES} TYPE INCLUDE)
install(DIRECTORY shaders     TYPE DATA)
install(DIRECTORY assets      TYPE DATA)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_PACKAGE_VERSION_MAJOR "${Grend_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${Grend_VERSION_MINOR}")
include(CPack)