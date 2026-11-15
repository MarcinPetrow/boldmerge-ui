canonical_tokens = files('boldmerge-tokens.css')

boldmerge_resources = gnome.compile_resources(
  'boldmerge-resources',
  'boldmerge.gresource.xml',
  source_dir: meson.current_source_dir(),
  c_name: 'boldmerge',
)

install_data(canonical_tokens, install_dir: boldmerge_datadir)
install_data('../../THIRD_PARTY_NOTICES.md', install_dir: boldmerge_datadir)
