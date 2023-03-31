project('dagger', 'c', 'cpp', default_options : ['cpp_std=c++17' ])

if (host_machine.system() == 'linux') # Needed to compile FPNG.cpp
    add_global_arguments('-march=native', language : 'c')
    add_global_arguments('-march=native', language : 'cpp')
endif

deps = [
    dependency('entt'),
    dependency('glm'),
    dependency('spdlog'),
    dependency('sdl2')
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
    'src/Main.cpp',
    'src/Platform.cpp',
    'src/Gravity.cpp',
    'src/Pickup.cpp',
    'src/PlayerControls.cpp',
    'src/Combat.cpp'
]

executable('dagger', sources, dependencies : deps, include_directories : include_directories('include'), cpp_args : '-DIMGUI_IMPL_OPENGL_LOADER_GLAD')
