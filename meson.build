project('utf8', 'c',
  version : '0.0.1',
  license : 'MIT')

inc = include_directories('include')

subdir('include')
subdir('src')

dlvm_utf8 = declare_dependency(link_with : utf8, include_directories : inc)

#executable('utf8', 'main.c', link_with : utf8, include_directories : inc)
