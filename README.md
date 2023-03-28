# Dagger setup guide

## Install needed system commands and dependencies:

### Windows:

Install [Visual Studio](https://visualstudio.microsoft.com/vs/) with enabled "Desktop development with C++" option. Afterwards run the following commands:

```
winget install python3 git.git meson
```

### macOS:
```sh
brew install meson sdl2 yaml-cpp
```

### Ubuntu 22.04:
```sh
sudo apt install git meson ninja-build clang libyaml-cpp-dev libsdl2-dev
```

### Fedora 37:
```sh
sudo dnf install git meson ninja-build clang yaml-cpp-devel SDL2-devel
```

## Clone the repository

```sh
git clone --recursive https://github.com/gavrilovmiroslav/dagger-relic.git
```

## Grab an IDE

We support using Visual Studio Code to do our development, you can grab it from the following link:
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
- Select the proper debug configuration in the debug panel on the left sidebar, "Dagger (Linux)", "Dagger (Windows)" or "Dagger (macOS)"
- Press F5 to start compilation / debug process
- Grofit!
