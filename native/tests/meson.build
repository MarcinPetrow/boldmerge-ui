foundation_test = executable(
  'test-foundation',
  'test-foundation.c',
  dependencies: boldmerge_dep,
)
test('foundation', foundation_test)

gtk_smoke_test = executable(
  'test-gtk-smoke',
  'test-gtk-smoke.c',
  dependencies: boldmerge_dep,
)
test('gtk-smoke', gtk_smoke_test)

component_test = executable(
  'test-components',
  'test-components.c',
  dependencies: boldmerge_dep,
)
test('components', component_test)

if get_option('demo')
  python = import('python').find_installation('python3')
  test(
    'accessibility-tree',
    python,
    args: [files('test-accessibility.py'), native_demo],
    env: ['GTK_A11Y=atspi'],
    timeout: 30,
  )
endif

visual_fixture = executable(
  'capture-visual',
  'visual/capture-visual.c',
  dependencies: boldmerge_dep,
)

shell = find_program('sh')
run_target(
  'capture-visuals',
  command: [
    shell,
    files('visual/capture.sh'),
    visual_fixture,
    meson.current_build_dir() / 'visual-output',
  ],
)
