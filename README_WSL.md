# Setup Windows development with Windows Subsystem for Linux

## Enable WSL

Open PowerShell as Administrator (Start menu > PowerShell > right-click > Run as Administrator) and enter the following commands:
```
dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
```

Now reboot your system.

## Configure WSL

Go to `%UserProfile%` in the explorer
Create a file `.wslconfig` in there with the following contents:
```
[wsl2]
memory=6GB
```

Now open powershell and type in the following commands:
```
wsl --update
wsl --set-default-version 2
wsl --shutdown
```

## Intall Ubuntu 22.04 app for Windows

- Open up Microsoft store
- Search for Ubuntu 22.04
- Install it
- Open and run the following commands in it:
```
sudo apt update
sudo apt install git meson ninja-build clang libyaml-cpp-dev libsdl2-dev gdb
```

## Clone the repository

Run the following command in Ubuntu 22.04 terminal:
```sh
git clone --recursive https://github.com/gavrilovmiroslav/dagger-relic.git
```

## Grab an IDE

We support using Visual Studio Code to do our development, you can grab it from the following link:
https://code.visualstudio.com/download

Afterwards simply run the following commands in the powershell to get the needed extensions to work with the project:
```sh
code --install-extension ms-vscode-remote.remote-wsl
code --install-extension ms-vscode.cpptools
code --install-extension mesonbuild.mesonbuild
```

## Compile and Run

- Open VS Code
- press Ctrl + P, type in `> WSL: New WSL Window` and run it
- Wait for it to load up
- press Ctrl + P, type in `Remote: Install Local Extensions in WSL: Ubuntu 22.04` and run it
- Select C++ and Meson extensions
- Press the OK button
- Now open the dagger-relic folder in VS Code
- Select the proper debug configuration in the debug panel on the left sidebar, in this case its "Dagger (Linux)" as we are actually running this on the Linux VM using WSL
- Press F5 to start compilation / debug process
- Grofit!
