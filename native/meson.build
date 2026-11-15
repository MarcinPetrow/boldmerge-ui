project(
  'boldmerge-native',
  'c',
  version: '0.0.0',
  meson_version: '>= 0.61.0',
  default_options: ['c_std=c11', 'warning_level=2'],
)

gnome = import('gnome')
pkgconfig = import('pkgconfig')

gtk_dep = dependency('gtk4', version: '>= 4.0')
adw_dep = dependency('libadwaita-1', version: '>= 1.0')
math_dep = meson.get_compiler('c').find_library('m', required: false)

boldmerge_datadir = get_option('datadir') / 'boldmerge-1'
boldmerge_includedir = get_option('includedir') / 'boldmerge-1'

subdir('data')
subdir('src')

install_headers(
  'include/boldmerge/boldmerge.h',
  'include/boldmerge/bm-theme-scope.h',
  'include/boldmerge/bm-icon.h',
  'include/boldmerge/bm-button.h',
  'include/boldmerge/bm-input.h',
  'include/boldmerge/bm-card.h',
  'include/boldmerge/bm-checkbox.h',
  'include/boldmerge/bm-radio.h',
  'include/boldmerge/bm-switch.h',
  'include/boldmerge/bm-select.h',
  'include/boldmerge/bm-icon-button.h',
  'include/boldmerge/bm-tag.h',
  'include/boldmerge/bm-status.h',
  'include/boldmerge/bm-page-layout.h',
  'include/boldmerge/bm-section.h',
  'include/boldmerge/bm-breadcrumbs.h',
  'include/boldmerge/bm-tabs.h',
  'include/boldmerge/bm-segmented-control.h',
  'include/boldmerge/bm-sidebar.h',
  'include/boldmerge/bm-topbar.h',
  'include/boldmerge/bm-app-shell.h',
  'include/boldmerge/bm-tooltip.h',
  'include/boldmerge/bm-dropdown.h',
  'include/boldmerge/bm-dialog.h',
  'include/boldmerge/bm-drawer.h',
  'include/boldmerge/bm-toast.h',
  'include/boldmerge/bm-validation-summary.h',
  'include/boldmerge/bm-combobox.h',
  'include/boldmerge/bm-command-palette.h',
  'include/boldmerge/bm-pagination.h',
  'include/boldmerge/bm-table.h',
  'include/boldmerge/bm-avatar.h',
  'include/boldmerge/bm-empty-state.h',
  'include/boldmerge/bm-skeleton.h',
  'include/boldmerge/bm-kpi.h',
  'include/boldmerge/bm-progress.h',
  'include/boldmerge/bm-chart.h',
  'include/boldmerge/bm-version.h',
  subdir: 'boldmerge-1/boldmerge',
)

pkgconfig.generate(
  libboldmerge,
  name: 'BoldMerge Native',
  filebase: 'boldmerge-1',
  description: 'BoldMerge Adaptive GTK 4 foundation',
  subdirs: 'boldmerge-1',
  requires: ['gtk4 >= 4.0', 'libadwaita-1 >= 1.0'],
)

gir_option = get_option('introspection')
gir_dep = dependency('gobject-introspection-1.0', required: gir_option)
if gir_dep.found()
  boldmerge_gir = gnome.generate_gir(
    libboldmerge,
    sources: boldmerge_public_headers + boldmerge_sources,
    namespace: 'Bm',
    nsversion: '1.0',
    identifier_prefix: 'Bm',
    symbol_prefix: 'bm',
    includes: ['Gtk-4.0', 'Adw-1'],
    install: true,
    extra_args: ['--warn-all'],
  )
elif gir_option.enabled()
  error('Introspection was enabled, but gobject-introspection-1.0 was not found')
endif

if get_option('demo')
  subdir('demo')
endif

if get_option('tests')
  subdir('tests')
endif
