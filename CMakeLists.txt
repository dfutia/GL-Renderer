# CMakeList.txt : CMake project for GLRenderer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("GLRenderer")

find_package(OpenGL REQUIRED)
find_package(Freetype CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(Bullet CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Lua REQUIRED)
find_package(imgui CONFIG REQUIRED)

find_path(THOMASMONKMAN_FILEWATCH_INCLUDE_DIRS "thomasmonkman-filewatch/FileWatch.hpp")

# Add source to this project's executable.
add_executable (GLRenderer "Main.cpp"  "Graphics/VertexBuffer.h" "Graphics/UniformBuffer.h" "Graphics/ShaderProgram.h" "Graphics/Shader.h" "Graphics/RenderBuffer.h" "Rendering/Material.h" "Graphics/FrameBuffer.h"  "Platform/Window.h" "Platform/Window.cpp" "Graphics/VertexBuffer.cpp" "Graphics/VertexArray.cpp" "Platform/File.h" "PCH.h" "Rendering/Material.cpp" "Graphics/FrameBuffer.cpp" "Rendering/Model.h" "Rendering/Mesh.h" "Rendering/Model.cpp" "Rendering/Mesh.cpp" "Animation/Animation.h" "Animation/Animation.cpp" "Graphics/Shader.cpp" "Graphics/ShaderProgram.cpp" "Graphics/Texture.cpp" "Graphics/GraphicsDevice.h" "Graphics/GraphicsDevice.cpp" "Rendering/Light.h" "Rendering/Camera.h" "Rendering/Skybox.h" "Rendering/Skybox.cpp" "Physics/PhysicsWorld.h" "Actors/PhysicsComponent.h" "Physics/PhysicsWorld.cpp" "Actors/PhysicsComponent.cpp" "ImguiBackend/imgui_impl_sdl2.h" "ImguiBackend/imgui_impl_sdl2.cpp" "Actors/LuaScriptComponent.h")

target_precompile_headers(GLRenderer PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/PCH.h>"
)

target_sources(GLRenderer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ImguiBackend/imgui_impl_sdl2.cpp
)

target_include_directories(GLRenderer PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/ImguiBackend
    ${THOMASMONKMAN_FILEWATCH_INCLUDE_DIRS} 
    ${Stb_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
)

target_compile_definitions(GLRenderer PUBLIC
    IMGUI_IMPL_OPENGL_LOADER_GLAD
)

target_link_libraries(GLRenderer PUBLIC
    glm::glm
    assimp::assimp
    OpenGL::GL   
    glad::glad   
    Freetype::Freetype   
    nlohmann_json::nlohmann_json
    ZLIB::ZLIB
    imgui::imgui
    ${BULLET_LIBRARIES}
    ${LUA_LIBRARIES}
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static> 
)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GLRenderer PROPERTY CXX_STANDARD 23)
endif()

# TODO: Add tests and install targets if needed.
