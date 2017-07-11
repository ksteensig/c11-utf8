project('utf8', 'c',
  version : '0.0.1',
  license : 'MIT')

add_global_arguments('-std=c11', language : 'c')

inc = include_directories('include')

subdir('include')
subdir('src')

executable('utf8', 'main.c', link_with : utf8, include_directories : inc)