# Setting up development environment on macOS / Linux

## Install needed system commands and dependencies:

macOS:
```sh
brew install meson ninja-build clang sdl2 yaml-cpp
```

Ubuntu 22.04:
```sh
sudo apt install git meson ninja-build clang libyaml-cpp-dev libsdl2-dev
```

Fedora 37:
```
sudo dnf install git meson ninja-build clang yaml-cpp-devel SDL2-devel
```

## Clone the repository

```sh
git clone --recursive https://github.com/gavrilovmiroslav/dagger-relic.git
```

## Grab an IDE

On macOS / Linux we support using Visual Studio Code to do our development, you can grab it from the following link:
https://code.visualstudio.com/download

Afterwards simply run the following commands in the terminal to get the needed extensions to work with the project:
```sh
code --install-extension ms-vscode.cpptools
code --install-extension mesonbuild.mesonbuild
```

## Compile and Run

- Open the dagger-relic folder in VS Code
- Wait for the meson extnsion to finish loading
  - It will take a bit longer for the extension to load when opening the folder for the first time as it will prepare all the project and dependenies for compilation
- Select the proper debug configuration in the debug panel on the left sidebar, "Dagger (Linux)" or "Dagger (macOS)"
- Press F5 to start compilation / debug process
- Grofit!
