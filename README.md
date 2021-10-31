# SoundBuddy

[![CodeFactor](https://www.codefactor.io/repository/github/amphaal/SoundBuddy/badge)](https://www.codefactor.io/repository/github/amphaal/SoundBuddy)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/b569eb64104e42589fd8825098562243)](https://www.codacy.com/gh/Amphaal/SoundBuddy/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Amphaal/SoundBuddy&amp;utm_campaign=Badge_Grade)

Small companion app that feeds SoundVitrine with ITunes / Music library metadata !

## How to build and debug

Available for MacOS (BigSur 11.0+) and Windows 10+. Usage of Visual Studio Code (https://code.visualstudio.com) as IDE is recommanded.
Requires a C++20 enabled compiler. We assume you have Git installed on your system.

- Run command in terminal to clone this repository: "git clone --recurse-submodules -j8 https://github.com/Amphaal/SoundBuddy"
- In VS Code, use the command palette to run "Tasks : Run Tests Task", which will configure the build
- In VS Code, simply run "Launch" in the "Execute and Debug" tab to build the executable 

SoundBuddy have some dependencies that are external and thus need to be installed on your system. Here is how to proceed on each handled platform :

### MacOS (LLVMClang)

All dependencies are fetched via Homebrew (https://brew.sh), make sure you have it properly installed on your build system.

    - Run command in terminal to fetch depedencies : "brew install ninja cmake llvm openssl@3 qt asio spdlog catch2"

### Windows (MSYS MinGW)

We use MSYS2 build system (https://www.msys2.org) as our build platform. Make sure to install it if it is not.

    - Define "MINGW64_ROOT" as environment variable, as the path of MinGW64 installed by MSYS2 (Ex: "C:\msys2\mingw64").
    - From VS Code, use the "Msys2 Shell" terminal to run the following commands.
    - Run command in terminal to update obsolete packages : "pacman -Syyu"
    - Run command in terminal to fetch depedencies : "pacman -S ninja cmake llvm openssl qt6 asio spdlog catch2"

## License
    SoundBuddy
    Small companion app that feeds SoundVitrine with ITunes / Music library metadata
    Copyright (C) 2018-2021 Guillaume Vara <guillaume.vara@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    Any graphical resources available within the source code may
    use a different license and copyright : please refer to their metadata
    for further details. Graphical resources without explicit references to a
    different license and copyright still refer to this GPL.
