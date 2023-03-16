project('dagger', 'c', 'cpp', default_options : ['cpp_std=c++17' ])

if (host_machine.system() == 'linux') # Needed to compile FPNG.cpp
    add_global_arguments('-march=native', language : 'c')
    add_global_arguments('-march=native', language : 'cpp')
endif

deps = [
    dependency('entt'),
    dependency('glm'),
    dependency('spdlog'),
    dependency('sdl2'),
    dependency('yaml-0.1'),
    dependency('imgui'),
    dependency('yaml-cpp')
]

includes = [
    include_directories('include')
]

sources = [
    'src/Access.cpp',
    'src/Animations.cpp',
    'src/Assets.cpp',
    'src/Config.cpp',
    'src/DaggerEditorTool.cpp',
    'src/DataBrowserTool.cpp',
    'src/Diagnostics.cpp',
    'src/DiagnosticsTool.cpp',
    'src/EventBus.cpp',
    'src/FPNG.cpp',
    'src/FPSCalculator.cpp',
    'src/ImagePreviewTool.cpp',
    'src/ImguiTextEditor.cpp',
    'src/Loaders.cpp',
    'src/LodePNG.cpp',
    'src/Rendering.cpp',
    'src/SpriteEditorTool.cpp',
    'src/SpriteRendering.cpp',
    'src/TextEditorTool.cpp',
    'src/Tooling.cpp',
    'src/Windowing.cpp',
    'src/Main.cpp'
]

executable('dagger', sources, dependencies : deps, include_directories : include_directories('include'), cpp_args : '-DIMGUI_IMPL_OPENGL_LOADER_GLAD')
