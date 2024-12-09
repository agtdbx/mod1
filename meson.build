project('mod1', 'cpp',
  version : '0.1',
  default_options : ['warning_level=1', 'cpp_std=c++14'])



conf_data = configuration_data()
conf_data.set('DATA_PATH', get_option('prefix') + '/data')




executable('mod1',
          'srcs/engine/maths/Random.cpp',
          'srcs/engine/maths/Vec3.cpp',
          'srcs/engine/maths/Vec2.cpp',
          'srcs/engine/inputs/Mouse.cpp',
          'srcs/engine/inputs/Key.cpp',
          'srcs/engine/inputs/InputManager.cpp',
          'srcs/parsing.cpp',
          'srcs/main.cpp',
		   build_rpath: '.',
           dependencies : [
              dependency('glfw3'),
              dependency('glew'),
              ],
			include_directories: include_directories('srcs'),
           install : true)

install_subdir('data', install_dir:'.')
