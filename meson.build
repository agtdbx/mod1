project('mod1', 'cpp',
  version : '0.1',
  default_options : ['warning_level=1', 'cpp_std=c++14'])

add_global_arguments('-g', language : 'cpp')

executable('mod1',
          'srcs/engine/OpenGLContext.cpp',
          'srcs/engine/inputs/InputManager.cpp',
          'srcs/engine/inputs/Key.cpp',
          'srcs/engine/inputs/Mouse.cpp',
          'srcs/engine/maths/Random.cpp',
          'srcs/engine/maths/Vec2.cpp',
          'srcs/engine/maths/Vec3.cpp',
          'srcs/engine/maths/utils.cpp',
          'srcs/engine/render/Camera.cpp',
          'srcs/engine/render/Mesh.cpp',
          'srcs/engine/render/Shader.cpp',
          'srcs/engine/render/TextureManager.cpp',
          'srcs/main.cpp',
          'srcs/model/Point.cpp',
          'srcs/model/Terrain.cpp',
          'srcs/model/Water.cpp',
          'srcs/model/WaterManager.cpp',
          'srcs/parsing.cpp',
          build_rpath: '.',
          dependencies : [
            dependency('glfw3'),
            dependency('glew'),
            dependency('glm'),
          ],
          include_directories: include_directories('srcs'),
          install : true)

install_subdir('data', install_dir:'.')
install_subdir('map', install_dir:'.')
install_data('vsupp', install_dir:'.')
