# Clementine's Geometry Toolbox (CSD3150)

# About
This is a solution file adapted from the framework
for CSD3150 given by Prasanna Ghali and Pushpak Karnick at DigiPen Singapore.

Currently, it uses Visual Studio configuration files to compile and build as the test development is on Windows 10.
There were issues with compiling the CMake version and work needed to be done earlier, so this is the solution for the time being. 

# Build instructions

## Windows 10
Open the solution file with Microsoft Visual Studios and simply hit 'build solution'.
A build output may also be created through the `cmd` or `powershell` using [MSBuild](https://docs.microsoft.com/en-us/visualstudio/msbuild/msbuild?view=vs-2022)

```cmd
# MSBuild.exe may need to be set in PATH
MSBuild.exe GeometryToolbox.sln /property:Configuration=Debug

# Alternative example based off default Microsoft Visual Studios install
"C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin\amd64\MSBuild.exe" OrangeEngine.sln /property:Configuration=Debug

```

# Depedencies

Name           | Link
----------------------- | ------------------------------------------
OpenGL Function Loader  | [glew](https://github.com/nigels-com/glew)
Windowing and Input     | [glfw](https://github.com/glfw/glfw)
OpenGL Mathematics      | [glm](https://github.com/g-truc/glm)

# Assignments

1. [Readme for Assignment 1](\Assignment1\Readme.md)