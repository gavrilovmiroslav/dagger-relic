project('dagger', 'c', 'cpp', default_options : [ 'cpp_std=c++17', 'default_library=static' ])

if host_machine.cpu() == 'x86_64' and meson.get_compiler('c').get_id() != 'msvc' # Needed to compile FPNG.cpp and SDL2
    add_global_arguments('-march=native', language : 'c')
    add_global_arguments('-march=native', language : 'cpp')
endif

deps = [
    dependency('entt'),
    dependency('glm'),
    dependency('spdlog'),
    dependency('sdl2', default_options : [ 'test=false' ])
]

includes = [
    include_directories('include')
]

sources = [
    'src/Access.cpp',
    'src/Animations.cpp',
    'src/Assets.cpp',
    'src/Config.cpp',
    'src/Diagnostics.cpp',
    'src/EventBus.cpp',
    'src/FPNG.cpp',
    'src/FPSCalculator.cpp',
    'src/Loaders.cpp',
    'src/LodePNG.cpp',
    'src/Rendering.cpp',
    'src/SpriteRendering.cpp',
    'src/Windowing.cpp',
    'src/PostProcessRendering.cpp',
    'src/Main.cpp'
]

executable('dagger',
    sources,
    dependencies : deps,
    include_directories : include_directories('include'),
    win_subsystem: 'windows')
