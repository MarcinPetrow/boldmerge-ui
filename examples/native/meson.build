project(
  'boldmerge-native-demo-app',
  'c',
  version: '0.0.0',
  meson_version: '>= 0.61.0',
  default_options: ['c_std=c11', 'warning_level=2'],
)

boldmerge_dep = dependency('boldmerge-1', version: '>= 0.0.0')

executable(
  'boldmerge-native-demo',
  'main.c',
  dependencies: boldmerge_dep,
)
