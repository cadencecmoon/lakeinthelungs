vorbis_sources = [
  'analysis.c',
  'bitrate.c',
  'block.c',
  'codebook.c',
  'envelope.c',
  'floor0.c',
  'floor1.c',
  'info.c',
  'lookup.c',
  'lpc.c',
  'lsp.c',
  'mapping0.c',
  'mdct.c',
  'psy.c',
  'registry.c',
  'res0.c',
  'sharedbook.c',
  'smallft.c',
  'synthesis.c',
  'window.c',
]

vorbisfile_sources = ['vorbisfile.c']
vorbisenc_sources = ['vorbisenc.c']

dep_math = meson.get_compiler('c').find_library('m', required: false)
pkgconfig = import('pkgconfig')

lib_vorbis = library(
  'vorbis',
  vorbis_sources,
  include_directories: [include_dir],
  dependencies: [dep_ogg, dep_math],
  vs_module_defs: '../vorbis.def',
  version: lib_version,
  soversion: lib_soversion,
  install: true,
)

dep_vorbis = declare_dependency(
  link_with: dep_vorbis,
  include_directories: include_dir,
  dependencies: [dep_ogg, dep_math],
)

pkgconfig.generate(
  lib_vorbis,
  description: 'vorbis is the primary Ogg Vorbis library',
)

lib_vorbisfile = library(
  'vorbisfile',
  vorbisfile_sources,
  include_directories: [include_dir],
  link_with: lib_vorbis,
  dependencies: [dep_ogg, dep_math],
  vs_module_defs: '../vorbisfile.def',
  version: vorbisfile_version,
  soversion: vorbisfile_soversion,
  install: true,
)

dep_vorbisfile = declare_dependency(
  link_with: lib_vorbisfile,
  include_directories: include_dir,
)

pkgconfig.generate(
  lib_vorbisfile,
  description: 'vorbisfile is a library that provides a convenient high-level API for decoding and basic manipulation of all Vorbis I audio streams',
)

lib_vorbisenc = library(
  'vorbisenc',
  vorbisenc_sources,
  include_directories: [include_dir],
  link_with: lib_vorbis,
  dependencies: [dep_ogg, dep_math],
  vs_module_defs: '../vorbisenc.def',
  version: vorbisenc_version,
  soversion: vorbisenc_soversion,
  install: true,
)

dep_vorbisenc = declare_dependency(
  link_with: lib_vorbisenc,
  include_directories: include_dir,
)

pkgconfig.generate(
  lib_vorbisenc,
  description: 'vorbisenc is a library that provides a convenient API for setting up an encoding environment using libvorbis',
)

test_sharedbook = executable(
  'test_sharedbook',
  'sharedbook.c',
  c_args: '-D_V_SELFTEST',
  include_directories: [include_dir],
  link_with: lib_vorbis,
  dependencies: [dep_ogg, dep_math],
)

test('sharedbook', test_sharedbook)
