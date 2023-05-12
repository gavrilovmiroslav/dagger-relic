project('dagger', 'c', 'cpp', default_options : [ 'cpp_std=c++17', 'default_library=static' ])

if host_machine.cpu() == 'x86_64' and meson.get_compiler('c').get_id() != 'msvc' # Needed to compile SDL2
    add_global_arguments('-march=native', language : 'c')
    add_global_arguments('-march=native', language : 'cpp')
endif

deps = [
    dependency('entt'),
    dependency('glm'),
    dependency('spdlog', default_options : [ 'tests=false' ]),
    dependency('sdl2', default_options : [ 'test=false' ]),
    dependency('sdl2_image'),
    dependency('sdl2_net'),
    dependency('sdl2_mixer'),
    dependency('sdl2_ttf')
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
    'src/FPSCalculator.cpp',
    'src/LevelManager.cpp',
    'src/Loaders.cpp',
    'src/PostProcessRendering.cpp',
    'src/Rendering.cpp',
    'src/SpriteRendering.cpp',
    'src/Windowing.cpp',
    'src/Main.cpp'
]

executable('dagger',
    sources,
    dependencies : deps,
    include_directories : include_directories('include'),
    win_subsystem: 'windows')
